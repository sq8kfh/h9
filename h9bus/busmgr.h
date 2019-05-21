/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include <utility>

#ifndef _H9_BUS_H_
#define _H9_BUS_H_

#include "config.h"
#include <map>
#include <queue>
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
private:
    SocketMgr * const _socket_mgr;
    std::queue<std::tuple<bool, std::string, H9frame>> frame_queue; //RECV/!SEND, bus id, frame

    std::map<std::string, Driver*> dev;
    Log frame_log;

    void recv_frame_callback(const H9frame& frame, const std::string& bus_id);
    void send_frame_callback(const H9frame& frame, const std::string& bus_id);
    void driver_close_callback(const std::string& bus_id);
    EventCallback create_event_callback(const std::string& bus_id);

    void send_turned_on_broadcast();

    std::string frame_to_log_string(const std::string& bus_id, const H9frame& frame);
public:
    explicit BusMgr(SocketMgr *socket_mgr);
    void load_config(Ctx *ctx);
    std::queue<std::tuple<bool, std::string, H9frame>>& get_recv_queue();
    void send_frame(const H9frame& frame, const std::string& bus_id = std::string("*"));
};


#endif //_H9_BUS_H_
