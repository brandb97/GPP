#include <iostream>
#include <unistd.h>
#include "gpp.hpp"

void goroutine1() {
    for (int i = 0; i < 10; i++) {
        std::cerr << "A";
    }
}

void goroutine2() {
    for (int i = 0; i < 10; i++) {
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
