#ifndef _H9_LOOP_H_
#define _H9_LOOP_H_

#include <netinet/in.h>

#include "h9bus/driver.h"

class Loop: public Driver {
private:
    sockaddr_in loopback_addr;
public:
    Loop(const std::string &bus_id);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_H9_LOOP_H_
