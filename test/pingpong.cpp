#include <iostream>
#include "gpp.hpp"

gpp::WaitGroup wg;

void ping() {
    for (int i = 0; i < 3; ++i) {
        std::cout << "ping\n";
        gpp::Sched();
    }
    wg.done();
}

void pong() {
    for (int i = 0; i < 3; ++i) {
        std::cout << "pong\n";
        gpp::Sched();
    }
    wg.done();
}

void pingpong() {
    wg.add(2);
    go(ping);
    go(pong);
    wg.wait();
}

int main() {
    gpp::SetNProcs(1);
    pingpong();
    return 0;
}
