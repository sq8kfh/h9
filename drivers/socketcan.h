#ifndef _SOCKETCAN_H_
#define _SOCKETCAN_H_

#include "h9bus/driver.h"


class SocketCAN: public Driver {
public:
    explicit SocketCAN(BusMgr::EventCallback event_callback);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_SOCKETCAN_H_