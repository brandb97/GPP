#ifndef _GPP_TLS_H_
#define _GPP_TLS_H_

struct TLS {
    struct GPP *g;
};

extern "C" GPP *getg();
extern "C" void setg(GPP *g);

#endif // _GPP_TLS_H_
