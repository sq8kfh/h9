#ifndef _H9_DUMMY_H_
#define _H9_DUMMY_H_

#include "h9bus/driver.h"

class Dummy: public Driver {
private:
    int write_only_socket;
public:
    explicit Dummy(BusMgr::EventCallback event_callback);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_H9_DUMMY_H_
