#ifndef DRIVER_H
#define DRIVER_H

#include <queue>
#include <string>

#include "socketmgr.h"
#include "busmgr.h"
#include "bus/h9frame.h"


class Driver: public SocketMgr::Socket {
private:
    std::queue<H9frame> send_queue;
    BusMgr::RecvFrameCallback _recv_frame_callback;
    BusMgr::SendFrameCallback _send_frame_callback;
protected:
    void on_frame_recv(const H9frame& frame);
    void on_frame_send(const H9frame& frame);

    virtual void recv_data() = 0;
    virtual void send_data(const H9frame& frame) = 0;
public:

    Driver(BusMgr::RecvFrameCallback recv_frame_callback, BusMgr::SendFrameCallback send_frame_callback);
    virtual void open() = 0;
    void close();

    void send_frame(const H9frame& frame);
    void on_select();
    ~Driver();
};


#endif //DRIVER_H
