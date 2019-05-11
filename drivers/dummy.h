#ifndef _H9_DUMMY_H_
#define _H9_DUMMY_H_

#include "h9bus/driver.h"

class Dummy: public Driver {
public:
    Dummy(const std::string &bus_id);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_H9_DUMMY_H_
