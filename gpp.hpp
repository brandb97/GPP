#ifndef _GPP_HPP_
#define _GPP_HPP_

#include <cstddef>
#include <mutex>
#include <vector>

extern void go(void (*fn)());

namespace gpp {
    extern void Sched();

    extern void Exit();

    extern void SetNProcs(int64_t n);

    class ConditionVariable {
    public:
        ConditionVariable() = default;

        void wait(std::mutex &mtx);
        void signal();
        void broadcast();
    private:
        std::vector< struct GPP * > waiters_;
    };

    class WaitGroup {
    public:
        WaitGroup() = default;

        void add(int delta);
        void done();
        void wait();
    private:
        std::mutex mtx_;
        ConditionVariable cv_;
        size_t counter_ = 0;
    };
}

#endif // _GPP_HPP_
