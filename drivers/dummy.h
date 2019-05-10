#ifndef _H9_DUMMY_H_
#define _H9_DUMMY_H_

#include "h9bus/driver.h"

class Dummy: public Driver {
public:
    Dummy();
    int open();
    void close();
    H9frame recv();
    void send(const H9frame& frame);
};


#endif //_H9_DUMMY_H_
