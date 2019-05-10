#ifndef DRIVER_H
#define DRIVER_H

#include <queue>

#include "socketmgr.h"
#include "bus/h9frame.h"

class Driver: public SocketMgr::Socket {
private:
    std::queue<H9frame> send_queue;
public:
    virtual int open() = 0;
    virtual void close() = 0;

    virtual H9frame recv() = 0;
    virtual void send(const H9frame& frame) = 0;

    void onSelect();
};


#endif //DRIVER_H
