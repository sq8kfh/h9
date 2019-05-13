#ifndef _H9_LOOP_H_
#define _H9_LOOP_H_

#include <netinet/in.h>

#include "h9bus/driver.h"

class Loop: public Driver {
private:
    constexpr static std::uint16_t LOOPBACK_PORT = 61432;

    sockaddr_in loopback_addr;
public:
    Loop(BusMgr::RecvFrameCallback recv_frame_callback, BusMgr::SendFrameCallback send_frame_callback);
    void open();
private:
    void recv_data();
    void send_data(const H9frame& frame);
};


#endif //_H9_LOOP_H_
