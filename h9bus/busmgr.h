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
        EventCallback(BusMgr *const bus_mgr, const std::string& bus_id): _bus_mgr(bus_mgr), _bus_id(bus_id) {};
        void on_fame_recv(const H9frame& frame);
        void on_fame_send(const H9frame& frame);
        void on_close();
    };
private:
    std::map<std::string, Driver*> dev;
    SocketMgr * const _socket_mgr;
    Log frame_log;

    void recv_frame_callback(const H9frame& frame, const std::string& bus_id);
    void send_frame_callback(const H9frame& frame, const std::string& bus_id);
    EventCallback create_event_callback(const std::string& bus_id);

    void send_turned_on_broadcast();

    std::string frame_to_log_string(const std::string& bus_id, const H9frame& frame);
public:
    BusMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);

    void send_frame(const H9frame& frame, const std::string& bus_id = std::string("*"));
};


#endif //_H9_BUS_H_
