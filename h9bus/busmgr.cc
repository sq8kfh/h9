/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "busmgr.h"

//#include <iostream>
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
    //TODO: auto reconnect
    dev.erase(bus_id);
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

BusMgr::BusMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr) {
}

void BusMgr::load_config(Ctx *ctx) {
    frame_log = ctx->log("h9frame");
    //Loop *loop = new Loop(std::move(create_event_callback("can0")));
    //dev["can0"] = loop;
    //Dummy *dummy = new Dummy(create_event_callback("can1"));
    //dev["can1"] = dummy;
    Slcan *slcan = new Slcan(create_event_callback("can2"), "/dev/tty.usbserial-DA002NQW");
    dev["can2"] = slcan;

    //loop->open();
    //_socket_mgr->register_socket(loop);
    //dummy->open();
    //_socket_mgr->register_socket(dummy);
    slcan->open();
    _socket_mgr->register_socket(slcan);

    /*H9frame tmp;
    tmp.priority = H9frame::Priority::LOW;
    tmp.source_id = 16;
    tmp.destination_id = 0;
    tmp.seqnum = 0;
    tmp.type = H9frame::Type::REG_VALUE;
    tmp.dlc = 2;
    tmp.data[0] = 0x0a;
    tmp.data[1] = 1;
    loop->send_frame(tmp);
    dummy->send_frame(tmp);
    slcan->send_frame(tmp);
    tmp.source_id = 32;
    slcan->send_frame(tmp);
    slcan->send_frame(tmp);
    slcan->send_frame(tmp);
    loop->send_frame(tmp);*/

    send_turned_on_broadcast();
}

BusMgr::EventCallback BusMgr::create_event_callback(const std::string &bus_id) {
    return BusMgr::EventCallback(this, bus_id);
}

void BusMgr::recv_frame_callback(const H9frame& frame, const std::string& bus_id) {
    frame_log.log(std::string("recv ") + frame_to_log_string(bus_id, frame));
    frame_queue.push(std::make_tuple(true, bus_id, frame));
}

void BusMgr::send_frame_callback(const H9frame& frame, const std::string& bus_id) {
    frame_log.log(std::string("send ") + frame_to_log_string(bus_id, frame));
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
