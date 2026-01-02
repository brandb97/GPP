#include "tls.hpp"

__thread TLS tls;

GPP *getg() {
    return tls.g;
}

void setg(GPP *g) {
    tls.g = g;
}
