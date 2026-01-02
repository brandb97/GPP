#include "proc.hpp"
#include "tls.hpp"
#include <cassert>
#include <unistd.h>

extern int main();

M m0;
GPP g0;

extern "C" {
    void mstart() {
        auto *gp = getg();
        auto *mp = gp->m;
        assert(mp->g0 == gp);
        mp->curg = nullptr;
        mp->tid = getpid();
        main();
    }
}
