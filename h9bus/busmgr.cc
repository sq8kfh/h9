/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "busmgr.h"

#include <sstream>
#include <iomanip>

#include "drivers/loop.h"
#include "drivers/dummy.h"
#include "drivers/slcan.h"
#include "drivers/socketcan.h"
#include "common/logger.h"


void BusMgr::EventCallback::on_fame_recv(const H9frame& frame) {
    _bus_mgr->recv_frame_callback(frame, _bus_id);
}

void BusMgr::EventCallback::on_fame_send(const H9frame& frame) {
    _bus_mgr->send_frame_callback(frame, _bus_id);
}

void BusMgr::EventCallback::on_close() {
    _bus_mgr->driver_close_callback(_bus_id);
}


void BusMgr::send_turned_on_broadcast() {
    H9frame cm;
    cm.priority = H9frame::Priority::LOW;
    cm.type = H9frame::Type::NODE_TURNED_ON;
    //TODO: read source_id from config file
    cm.source_id = 2;
    cm.destination_id = H9frame::BROADCAST_ID;
    cm.dlc = 0;

    send_frame(cm);
}

void BusMgr::driver_close_callback(const std::string& bus_id) {
    h9_log_warn("driver: %s closed", bus_id.c_str());
    _socket_mgr->unregister_socket(dev[bus_id]);
    //dev.erase(bus_id);
}

std::string BusMgr::frame_to_log_string(const std::string& bus_id, const H9frame& frame) {
    std::ostringstream frame_string;
    frame_string << bus_id
       << ": " << frame.source_id
       << "->" << frame.destination_id
       << "; priority: " << (frame.priority == H9frame::Priority::HIGH ? 'H' : 'L')
       << "; type: " << static_cast<unsigned int>(H9frame::to_underlying(frame.type))
       << "; seqnum: " << static_cast<unsigned int>(frame.seqnum)
       << "; dlc: " << static_cast<unsigned int>(frame.dlc)
       << "; data:";

    for (int i = 0; i < frame.dlc; ++i) {
        frame_string << ' ' << std::setfill('0') << std::hex << std::setw(2) << static_cast<unsigned int>(frame.data[i]);
    }
    frame_string << ';';
    return frame_string.str();
}

BusMgr::BusMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr), frame_log(nullptr) {
}

BusMgr::~BusMgr() {
    delete frame_log;
}

void BusMgr::load_config(BusCtx *ctx) {
    frame_log = new FrameLogger(ctx->cfg_log_send_logfile(),ctx->cfg_log_recv_logfile());

    for(std::string bus_name: ctx->cfg_bus_list()) {
        std::string driver = ctx->cfg_bus_driver(bus_name);
        std::string cs = ctx->cfg_bus_connection_string(bus_name);

        if (driver == "dummy") {
            Dummy *dummy = new Dummy(create_event_callback(bus_name));
            dev[bus_name] = dummy;
            dummy->open();
            _socket_mgr->register_socket(dummy);
        }
        else if (driver == "loop") {
            Loop *loop = new Loop(std::move(create_event_callback(bus_name)));
            dev[bus_name] = loop;
            loop->open();
            _socket_mgr->register_socket(loop);
        }
        else if (driver == "slcan") {
            Slcan *slcan = new Slcan(create_event_callback(bus_name), cs);
            dev[bus_name] = slcan;
            slcan->open();
            _socket_mgr->register_socket(slcan);
        }
#if defined(__linux__)
        else if (driver == "socketcan") {
            SocketCAN *socketcan = new SocketCAN(create_event_callback(bus_name), cs);
            dev[bus_name] = socketcan;
            socketcan->open();
            _socket_mgr->register_socket(socketcan);
        }
#endif
        else {
            h9_log_crit("Unsupported bus(%s) driver: %s", bus_name.c_str(), driver.c_str());
            exit(EXIT_FAILURE);
        }
    }

    send_turned_on_broadcast();
}

BusMgr::EventCallback BusMgr::create_event_callback(const std::string &bus_id) {
    return BusMgr::EventCallback(this, bus_id);
}

void BusMgr::recv_frame_callback(const H9frame& frame, const std::string& bus_id) {
    frame_log->log_recv(bus_id, frame);
    h9_log_debug(std::string("recv frame: ") + frame_to_log_string(bus_id, frame));
    frame_queue.push(std::make_tuple(true, bus_id, frame));
}

void BusMgr::send_frame_callback(const H9frame& frame, const std::string& bus_id) {
    frame_log->log_send(bus_id, frame);
    h9_log_debug(std::string("send frame: ") + frame_to_log_string(bus_id, frame));
    frame_queue.push(std::make_tuple(false, bus_id, frame));
}

std::queue<std::tuple<bool, std::string, H9frame>>& BusMgr::get_recv_queue() {
    return frame_queue;
}

void BusMgr::send_frame(const H9frame& frame, const std::string& bus_id) {
    if (bus_id == "*") {
        for (auto& it: dev) {
            it.second->send_frame(frame);
        }
    }
    else if (dev.count(bus_id) == 1){
        dev[bus_id]->send_frame(frame);
    }
}

void BusMgr::cron() {
    for (auto it=dev.begin(); it!=dev.end(); ++it) {
        if (!it->second->is_connected()) {
            if (it->second->retry_auto_connect) {
                try {
                    it->second->open();
                    _socket_mgr->register_socket(it->second);
                }
                catch (...) {
                    --it->second->retry_auto_connect;
                    if (it->second->retry_auto_connect == 0) {
                        it->second->on_close();
                    }
                }
            }
        }
    }
}

void BusMgr::flush_dev() {
    for (auto it = dev.cbegin(); it != dev.cend();) {
        if (!it->second->is_connected()) {
            Driver *tmp = it->second;
            _socket_mgr->unregister_socket(tmp);
            if (it->second->retry_auto_connect == 0) {
                h9_log_notice("BusMgr: flush driver %s (%d)",
                              it->first.c_str(),
                              tmp->get_socket());

                it = dev.erase(it);
                delete tmp;
            }
            else {
                ++it;
            }
        }
        else {
            ++it;
        }
    }
}

std::vector<std::string> BusMgr::get_dev_list() {
    std::vector<std::string> ret;
    for(auto& it: dev) {
        ret.push_back(it.first);
    }
    return std::move(ret);
}

std::uint32_t BusMgr::get_dev_counter(const std::string& dev_name, BusMgr::CounterType counter) {
    return dev[dev_name]->get_counter(counter);
}
