#ifndef _SOCKETCAN_H_
#define _SOCKETCAN_H_

#include "h9bus/driver.h"

class SocketCAN: public Driver {
public:
    SocketCAN();
    int open();
    void close();
    H9frame recv();
    void send(const H9frame& frame);
};


#endif //_SOCKETCAN_H_
