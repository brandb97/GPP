#include "proc.hpp"
#include "tls.hpp"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <sys/types.h>
#include <unistd.h>

extern int main();

M m0;
GPP g0;

static struct schedt {
    std::mutex lock;
    std::queue<GPP *> runq;
    
    int64_t nms;
    int64_t nidlems;
    int64_t nrunningms;
    std::vector<M *> allm;
    bool mainstarted;
} sched;

extern "C" void *mstart_stub(void *arg);

extern "C" void mcall(void (*fn)(void));

extern "C" void gogo(GPP *sched);

extern "C" void *getCallerPC();

extern "C" void *getCallerSP();

extern "C" void *getCallerBP();

extern "C" void systemstack(void (*fn)(void *arg), void *arg);

static void throw_internal(const char *msg) {
    // print out all the frames to stderr
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

void mpark() {
    auto *mp = getg()->m;
    pthread_mutex_lock(&mp->lock);
    if (--mp->count < 0) {
        pthread_cond_wait(&mp->cond, &mp->lock);
    }
    pthread_mutex_unlock(&mp->lock);
}

void munpark(M *mp) {
    pthread_mutex_lock(&mp->lock);
    if (++mp->count >= 0) {
        pthread_cond_signal(&mp->cond);
    }
    pthread_mutex_unlock(&mp->lock);
}

void stopm() {
    /* We must already hold sched.lock in caller
     * I don't know if there are better ways to
     * test `mustHeldLock` */
    assert(!sched.lock.try_lock());

    sched.nrunningms--;
    sched.nidlems++;
    auto *mp = getg()->m;
    mp->status = M::IDLE;

    if (sched.nrunningms == 0 && sched.runq.empty()) {
        throw_internal("all gpproutines are asleep - deadlock!");
    }
    sched.lock.unlock();
    mpark();
    sched.lock.lock();
    sched.nrunningms++;
    sched.nidlems--;
    mp->status = M::RUNNING;
}

void wakem() {
    /* We must already hold sched.lock in caller
     * I don't know if there are better ways to
     * test `mustHeldLock` */
    assert(!sched.lock.try_lock());

    if (sched.nidlems == 0 && sched.nms < GPPMAXPROC) {
        /* start new m */
        sched.nms++;
        sched.nrunningms++;
        M *mp = new M;
        mp->g0 = new GPP;
        mp->g0->m = mp;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, STACKSIZE);

        pthread_t tid;
        pthread_create(&tid, &attr, mstart_stub, mp);
        pthread_detach(tid);
    } else {
        // Wake up a random m, prefer idle m
        int rand = random();

        int target_idx = 0;
        auto target_status = M::IDLE;
        if (sched.nidlems > 0) {
            target_idx = rand % sched.nidlems;
            target_status = M::IDLE;
        } else {
            assert(sched.nrunningms > 0);
            target_idx = rand % sched.nrunningms;
            target_status = M::RUNNING;
        }

        for (auto *mp : sched.allm) {
            if (mp->status == target_status) {
                if (target_idx == 0) {
                    munpark(mp);
                    break;
                }
                target_idx--;
            }
        }
    }
}

void mexit() {
    auto *mp = getg()->m;
    if (mp == &m0) {
        sched.lock.lock();
        sched.nrunningms--;
        sched.nms--;
        if (sched.nrunningms == 0 && sched.runq.empty()) {
            throw_internal("all gpproutines are asleep - deadlock!");
        }
        sched.lock.unlock();
        mpark();
        throw_internal("wake up exited main thread!");
    }

    delete mp;
    sched.lock.lock();
    sched.nrunningms--;
    sched.nms--;
    if (sched.nrunningms == 0 && sched.runq.empty()) {
        throw_internal("all gpproutines are asleep - deadlock!");
    }
    sched.lock.unlock();
}

void schedule() {
    for (;;) {
        std::unique_lock< std::mutex > lk(sched.lock);
        if (sched.runq.empty()) {
            stopm();
            continue;
        }
        GPP *gp = sched.runq.front();
        sched.runq.pop();
        lk.unlock();

        auto *mp = getg()->m;
        mp->curg = gp;
        gp->m = mp;
        /* switch to gp */
        gogo(gp);
    }
}

static void mstart0(M *mp) {
    mp->g0->sched.sp = getCallerSP();
    mp->g0->sched.pc = getCallerPC();
    mp->g0->sched.bp = getCallerBP();

    schedule();
}

/* Must be on system stack */
static void gppexit_systemstack() {
    auto *g0 = getg();
    auto *gp = g0->m->curg;
    g0->m->curg = nullptr;

    delete[] gp->stack;
    delete gp;

    schedule();
}

static void gppexit() {
    auto *fn = reinterpret_cast<void (*)(void *)>(&gppexit_systemstack);
    systemstack(fn, nullptr);
}

static GPP *allocg(void (*fn)()) {
    auto *newg = new GPP;
    newg->m = nullptr;
    newg->stack = new char[STACKSIZE];
    newg->sched.sp = newg->stack + STACKSIZE - 8;
    newg->sched.pc = (void *)fn;
    newg->sched.bp = 0;

    *static_cast<uintptr_t *>(newg->sched.sp) = reinterpret_cast<uintptr_t>(&gppexit);
    return newg;
}

void newproc(void (*fn)()) {
    auto *newg = allocg(fn);
    sched.lock.lock();
    sched.runq.push(newg);
    sched.lock.unlock();

    if (sched.mainstarted) {
        sched.lock.lock();
        wakem();
        sched.lock.unlock();
    }
}

static void main_main() {
    sched.mainstarted = true;
    int ret = main();
    exit(ret);
}

extern "C" {
    void schedinit() {
        sched.nms = 1;
        sched.nidlems = 0;
        sched.nrunningms = 1;
        sched.mainstarted = false;
    }

    /* Assembly need to call this function
    * So it's name must not be mangled by C++ compiler.
    */
    void mstart() {
        auto *gp = getg();
        auto *mp = gp->m;
        assert(mp->g0 == gp);
        mp->curg = nullptr;
        mp->tid = pthread_self();

        mp->count = 0; /* not waiting */
        mp->lock = PTHREAD_MUTEX_INITIALIZER;
        mp->cond = PTHREAD_COND_INITIALIZER;

        /* Init g0.sched then call schedule()
         * Never returns */
        mstart0(mp);

        /* gogo(&mp->g0->sched) would reach here */
        mexit();
    }

    void mstart_main() {
        newproc(main_main);
        mstart();
    }
}
