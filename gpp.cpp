#include "proc.hpp"
#include "tls.hpp"
#include "gpp.hpp"
#include <cassert>

extern "C" void systemstack(void (*fn)(void *arg), void *arg);

void go(void (*fn)()) {
    // Allocate a new GPP
    auto *ptr = reinterpret_cast<void (*)(void *)>(&newproc);
    auto *arg = reinterpret_cast<void *>(fn);
    systemstack(ptr, arg);
}

namespace gpp {
    void Sched() {
        gppsched();
    }

    void Exit() {
        gppexit();
    }

    void ConditionVariable::wait(std::mutex &mtx) {
        auto *gp = getg();
        waiters_.push_back(gp);
        mtx.unlock();
        gpppark();
        mtx.lock();
    }

    void ConditionVariable::signal() {
        if (!waiters_.empty()) {
            auto *gp = waiters_.back();
            waiters_.pop_back();
            gppunpark(gp);
        }
    }

    void ConditionVariable::broadcast() {
        while (!waiters_.empty()) {
            auto *gp = waiters_.back();
            waiters_.pop_back();
            gppunpark(gp);
        }
    }

    void WaitGroup::add(int delta) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (delta < 0) {
            assert(counter_ >= static_cast<size_t>(-delta) && "negative WaitGroup counter");
        }
        counter_ += delta;
        if (counter_ == 0) {
            cv_.broadcast();
        }
    }

    void WaitGroup::done() {
        this->add(-1);
    }

    void WaitGroup::wait() {
        mtx_.lock();
        while (counter_ > 0) {
            cv_.wait(mtx_);
        }
    }
}
