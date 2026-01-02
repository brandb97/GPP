#include <iostream>
#include <unistd.h>
#include "gpp.hpp"

void goroutine1() {
    for (;;) {
        std::cerr << "A";
    }
}

void goroutine2() {
    for (;;) {
        std::cerr << "B";
    }
}

int main() {
    GPP(goroutine1);
    GPP(goroutine2);
    std::cerr << "Hello, World!" << std::endl;
    sleep(1);
    return 0;
}
