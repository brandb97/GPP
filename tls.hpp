#ifndef _GPP_TLS_HPP_
#define _GPP_TLS_HPP_

struct TLS {
    struct GPP *g;
};

extern "C" GPP *getg();
extern "C" void setg(GPP *g);

#endif // _GPP_TLS_HPP_