#ifndef _H9_LOOP_H_
#define _H9_LOOP_H_

#include <netinet/in.h>

#include "h9bus/driver.h"

class Loop: public Driver {
private:
    sockaddr_in loopback_addr;
public:
    Loop();
    int open();
    void close();
    H9frame recv();
    void send(const H9frame& frame);
};


#endif //_H9_LOOP_H_
