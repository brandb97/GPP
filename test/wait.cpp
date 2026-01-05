#include <iostream>
#include "gpp.hpp"

gpp::WaitGroup wg;

void worker() {
    std::cout << "Worker is running\n";
    std::cout << "Worker is done\n";
    wg.done();
}

int main() {
    gpp::SetNProcs(2);
    wg.add(1);
    go(worker);
    wg.wait();
}
