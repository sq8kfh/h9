#ifndef DRIVER_H
#define DRIVER_H

#include <queue>
#include <string>

#include "socketmgr.h"
#include "bus/h9frame.h"

class Driver: public SocketMgr::Socket {
private:
    std::queue<H9frame> send_queue;
    const std::string _bus_id;
protected:
    //virtual void send_next_msg(const H9frame& frame) = 0;

    void on_frame_recv(const H9frame& frame);
    void on_frame_send(const H9frame& frame);

    virtual void recv_data() = 0;
    virtual void send_data(const H9frame& frame) = 0;
public:
    Driver(const std::string &bus_id);
    virtual void open() = 0;
    void close();

    void send_frame(const H9frame& frame);
    void on_select();
    ~Driver();
};


#endif //DRIVER_H
