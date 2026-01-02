#ifndef _GPP_HPP_
#define _GPP_HPP_

#include <sys/types.h>

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
};

struct M {
    // Offset is 0, known to assembly code.
    struct GPP *g0;
    // Offset is 8, known to assembly code.
    struct GPP *curg;

    pid_t tid;
};

#endif // _GPP_HPP_
