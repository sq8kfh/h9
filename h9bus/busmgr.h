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
class EventMgr;

class BusMgr {
private:
    SocketMgr * const _socket_mgr;
    EventMgr* eventmgr_handler;

    std::map<std::string, Driver*> dev;
    FrameLogger *frame_log;

    void on_recv_frame(Driver *endpoint, const H9frame& frame);
    void on_send_frame(Driver *endpoint, BusFrame *busframe);
    void on_driver_close(const std::string& endpoint);

    std::string frame_to_log_string(const std::string& endpoint, const H9frame& frame);

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
    void set_eventmgr_handler(EventMgr* handler);

    void send_frame(const H9frame& frame, const std::string& origin, const std::string& endpoint = "");

    std::vector<std::string> get_dev_list();
    std::uint32_t get_dev_counter(const std::string& dev_name, CounterType counter);

    void cron();
    void flush_dev();
};


#endif //_H9_BUS_H_
