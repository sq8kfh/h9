#ifndef _H9_SLCAN_H_
#define _H9_SLCAN_H_

#include "drivers/driver.h"

class Slcan: public Driver {
    public:
        Slcan();
        int open();
        void close();
        H9frame recv();
        void send(const H9frame& frame);
};


#endif //_H9_SLCAN_H_
