#ifndef _GPP_PROC_HPP_
#define _GPP_PROC_HPP_

#include <cstdint>
#include <sys/types.h>
#include <pthread.h>

struct GPPState {
    // Offset is 0, known to assembly code.
    void *sp;

    // Offset is 8, known to assembly code.
    void *bp;

    // Offset is 16, known to assembly code.
    void *pc;
};

struct GPP {
    // Offset is 0, known to assembly code.
    struct M *m;

    // Offset is 8, known to assembly code.
    struct GPPState sched;

    char *stack = nullptr;
};

struct M {
    // Offset is 0, known to assembly code.
    struct GPP *g0;
    // Offset is 8, known to assembly code.
    struct GPP *curg;

    pthread_t tid;

    pthread_mutex_t lock;
    pthread_cond_t cond;
    int64_t count;

    enum {
        IDLE,
        RUNNING,
    } status;
};

/* MAX number of threads we allow */
constexpr int64_t GPPMAXPROC = 1;

/* 8 KB stack, go use this size for g0 stacks */
constexpr int64_t STACKSIZE = 8 << 10;

extern void mpark();
extern void munpark(M *mp);
extern void stopm();
extern void wakem();
extern "C" void mstart();
extern void mexit();
extern void schedule();
extern void newproc(void (*fn)());
extern void gppsched();
extern void gpppark();
extern void gppunpark(GPP *gp);

#endif // _GPP_PROC_HPP_
