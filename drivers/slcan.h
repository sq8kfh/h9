#ifndef _H9_SLCAN_H_
#define _H9_SLCAN_H_

#include "h9bus/driver.h"

class Slcan: public Driver {
public:
    Slcan(const std::string &bus_id);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_H9_SLCAN_H_
