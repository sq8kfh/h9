#include <utility>

#ifndef _H9_BUS_H_
#define _H9_BUS_H_

#include <map>

//#include "driver.h"
#include "socketmgr.h"
#include "common/ctx.h"
#include "bus/h9frame.h"


class Driver;

class BusMgr {
public:
    class EventCallback {
    private:
        BusMgr *const _bus_mgr;
        const std::string _bus_id;
    public:
        EventCallback(BusMgr *const bus_mgr, std::string bus_id): _bus_mgr(bus_mgr), _bus_id(std::move(bus_id)) {};
        void on_fame_recv(const H9frame& frame);
        void on_fame_send(const H9frame& frame);
        void on_close();
    };

    using msg_frame_callback_f = std::function<void(const std::string&, const H9frame&)>;
private:
    std::map<std::string, Driver*> dev;
    SocketMgr * const _socket_mgr;
    Log frame_log;

    msg_frame_callback_f _event_msg_frame_callback;

    void recv_frame_callback(const H9frame& frame, const std::string& bus_id);
    void send_frame_callback(const H9frame& frame, const std::string& bus_id);
    void driver_close_callback(const std::string& bus_id);
    EventCallback create_event_callback(const std::string& bus_id);

    void send_turned_on_broadcast();

    std::string frame_to_log_string(const std::string& bus_id, const H9frame& frame);
public:
    explicit BusMgr(SocketMgr *socket_mgr);
    void set_frame_recv_callback(msg_frame_callback_f event_msg_frame_callback);
    void load_config(Ctx *ctx);

    void send_frame(const H9frame& frame, const std::string& bus_id = std::string("*"));
};


#endif //_H9_BUS_H_
