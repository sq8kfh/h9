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
    std::uint8_t next_seqnum;

    BusMgr::EventCallback _event_callback;
protected:
    void on_frame_recv(const H9frame& frame);
    void on_frame_send(const H9frame& frame);

    virtual void recv_data() = 0;
    virtual void send_data(const H9frame& frame) = 0;
public:

    explicit Driver(BusMgr::EventCallback event_callback);
    virtual void open() = 0;
    void close();

    void send_frame(const H9frame& frame);
    void on_select();
    ~Driver();
};


#endif //DRIVER_H
