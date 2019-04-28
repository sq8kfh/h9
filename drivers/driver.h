#ifndef DRIVER_H
#define DRIVER_H

#include "bus/h9frame.h"

class Driver {
protected:
    int socket;
public:
    virtual int open() = 0;
    virtual void close() = 0;

    virtual H9frame recv() = 0;
    virtual void send(const H9frame& frame) = 0;
};


#endif //DRIVER_H
