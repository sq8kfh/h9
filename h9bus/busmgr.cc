#include "busmgr.h"

#include "drivers/loop.h"
#include "drivers/dummy.h"
#include "drivers/slcan.h"
#include "drivers/socketcan.h"

#include <iostream>

void BusMgr::RecvFrameCallback::operator()(const H9frame& frame) {
    _bus_mgr->recv_frame(frame, _bus_id);
}

BusMgr::BusMgr(SocketMgr *socket_mgr): _socket_mgr(socket_mgr) {
}

void BusMgr::load_config(Ctx *ctx) {
    Loop *loop = new Loop(create_recv_frame_callback("can0"));
    dev["can0"] = loop;
    Dummy *dummy = new Dummy(create_recv_frame_callback("can1"));
    dev["can1"] = dummy;
    Slcan *slcan = new Slcan(create_recv_frame_callback("can2"), "/dev/tty.usbserial-DA002NQW");
    dev["can2"] = slcan;

    loop->open();
    _socket_mgr->register_socket(loop);
    dummy->open();
    _socket_mgr->register_socket(dummy);
    slcan->open();
    _socket_mgr->register_socket(slcan);

    H9frame tmp;
    tmp.priority = H9frame::Priority::LOW;
    tmp.source_id = 16;
    tmp.destination_id = 0;
    tmp.seqnum = 0;
    tmp.type = H9frame::Type::REG_VALUE;
    tmp.dlc = 2;
    tmp.data[0] = 0x0a;
    tmp.data[1] = 1;
    //loop->send_frame(tmp);
    dummy->send_frame(tmp);
    slcan->send_frame(tmp);
    tmp.source_id = 32;
    slcan->send_frame(tmp);
}

BusMgr::RecvFrameCallback BusMgr::create_recv_frame_callback(const std::string &bus_id) {
    return BusMgr::RecvFrameCallback(this, bus_id);
}

void BusMgr::recv_frame(const H9frame& frame, const std::string& bus_id) {
    std::cout << "recv (" << bus_id << "): " << frame << std::endl;
}

void BusMgr::send_frame(const H9frame& frame, const std::string& bus_id) {
    if (bus_id.compare("*") == 0) {
        for (auto& it: dev) {
            it.second->send_frame(frame);
        }
    }
    else if (dev.count(bus_id) == 1){
        dev[bus_id]->send_frame(frame);
    }
}
