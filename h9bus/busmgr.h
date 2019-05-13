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
    class RecvFrameCallback {
    private:
        BusMgr *const _bus_mgr;
        const std::string _bus_id;
    public:
        RecvFrameCallback(BusMgr *const bus_mgr, const std::string& bus_id): _bus_mgr(bus_mgr), _bus_id(bus_id) {};
        void operator()(const H9frame& frame);
    };
    class SendFrameCallback {
    private:
        BusMgr *const _bus_mgr;
        const std::string _bus_id;
    public:
        SendFrameCallback(BusMgr *const bus_mgr, const std::string& bus_id): _bus_mgr(bus_mgr), _bus_id(bus_id) {};
        void operator()(const H9frame& frame);
    };
private:
    std::map<std::string, Driver*> dev;
    SocketMgr * const _socket_mgr;
    void recv_frame_callback(const H9frame& frame, const std::string& bus_id);
    void send_frame_callback(const H9frame& frame, const std::string& bus_id);
    SendFrameCallback create_send_frame_callback(const std::string& bus_id);
public:
    BusMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);
    RecvFrameCallback create_recv_frame_callback(const std::string& bus_id);

    void send_frame(const H9frame& frame, const std::string& bus_id = std::string("*"));
};


#endif //_H9_BUS_H_
