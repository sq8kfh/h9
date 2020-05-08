/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_BUS_H_
#define _H9_BUS_H_

#include "config.h"
#include <map>
#include <queue>
#include <utility>
#include <confuse.h>
#include "socketmgr.h"
#include "framelogger.h"
#include "busctx.h"
#include "bus/h9frame.h"
#include "busframe.h"


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
        void on_fame_send(BusFrame *busframe);
        void on_close();
    };
private:
    SocketMgr * const _socket_mgr;
    std::queue<std::tuple<std::string, std::string, H9frame>> frame_queue; //origin, endpoint, frame

    std::map<std::string, Driver*> dev;
    FrameLogger *frame_log;

    void recv_frame_callback(const H9frame& frame, const std::string& endpoint);
    void send_frame_callback(BusFrame *busframe, const std::string& endpoint);
    void driver_close_callback(const std::string& bus_id);
    EventCallback create_event_callback(const std::string& bus_id);

    void send_turned_on_broadcast();

    std::string frame_to_log_string(const std::string& bus_id, const H9frame& frame);

    static cfg_opt_t cfg_bus_sec[];
public:
    static cfg_opt_t cfg_section;
    enum class CounterType {
        SEND_FRAMES,
        RECEIVED_FRAMES,
    };

    explicit BusMgr(SocketMgr *socket_mgr);
    ~BusMgr();

    void load_config(BusCtx *ctx);
    std::queue<std::tuple<std::string, std::string, H9frame>>& get_recv_queue();
    void send_frame(const H9frame& frame, const std::string& origin, const std::string& endpoint = "");

    std::vector<std::string> get_dev_list();
    std::uint32_t get_dev_counter(const std::string& dev_name, CounterType counter);

    void cron();
    void flush_dev();
};


#endif //_H9_BUS_H_
