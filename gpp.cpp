#include "proc.hpp"
#include "gpp.hpp"

extern "C" void systemstack(void (*fn)(void *arg), void *arg);

void GPP(void (*fn)()) {
    // Allocate a new GPP
    auto *ptr = reinterpret_cast<void (*)(void *)>(&newproc);
    auto *arg = reinterpret_cast<void *>(fn);
    systemstack(ptr, arg);
}
