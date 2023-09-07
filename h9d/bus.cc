/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "bus.h"
#include <atomic>
#include <thread>
#include <netinet/in.h>
#include "node.h"
#include "protocol/errormsg.h"
#include "protocol/framemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/executemethodmsg.h"
#include "protocol/subscribemsg.h"
#include "src/bus.h"


void Bus::recv_thread() {
    while (run) {
        try {
            GenericMsg raw_msg = h9bus_connector->recv();
            if (raw_msg.get_type() == GenericMsg::Type::FRAME) {
                FrameMsg msg = std::move(raw_msg);
                H9frame frame = msg.get_frame();

                notify_frame_observer(frame);

                if (msg.get_request_id() != 0) {
                    send_promise_map_mtx.lock();
                    if (send_promise_map.count(msg.get_request_id())) {
                        send_promise_map[msg.get_request_id()].set_value(int(Bus::OK));
                        send_promise_map.erase(msg.get_request_id());
                    }
                    send_promise_map_mtx.unlock();
                }
            } else if (raw_msg.get_type() == GenericMsg::Type::ERROR) {
                ErrorMsg msg = std::move(raw_msg);
                h9_log_info("error frame %llu %llu", msg.get_id(), msg.get_request_id());
                if (msg.get_request_id() != 0) {
                    send_promise_map_mtx.lock();
                    if (send_promise_map.count(msg.get_request_id())) {
                        send_promise_map[msg.get_request_id()].set_value(int(Bus::H9BUS_ERROR));
                        send_promise_map.erase(msg.get_request_id());
                    }
                    send_promise_map_mtx.unlock();
                }
            }
        }
        catch (...) {
            if (run) throw;
            else return;
        }
    }
}

int Bus::send_frame_sync(H9frame frame) noexcept {
    std::promise<int> send_promise;
    std::future<int> send_future = send_promise.get_future();
    std::uint64_t msg_id = h9bus_connector->get_next_id();
    send_promise_map_mtx.lock();
    send_promise_map[msg_id] = std::move(send_promise);
    send_promise_map_mtx.unlock();

    try {
        h9bus_connector->send(SendFrameMsg(frame), msg_id);
    }
    catch (std::runtime_error &e) {
        h9_log_err("h9bus error during sending frame: %s", e.what());
        send_promise_map_mtx.lock();
        send_promise_map.erase(msg_id);
        send_promise_map_mtx.unlock();
        return NOT_CONNECTED_TO_H9BUS;
    }
    if (send_future.wait_for(std::chrono::seconds(send_timeout)) != std::future_status::ready) {
        send_promise_map_mtx.lock();
        send_promise_map.erase(msg_id);
        send_promise_map_mtx.unlock();
        return SEND_TIMEOUT;
    }
    return send_future.get();
}

Bus::Bus() noexcept: h9bus_connector(nullptr), run(true), send_timeout(DEFAULT_SEND_FRAME_TO_H9BUS_TIMEOUT) {
}

Bus::~Bus() noexcept {
    run = false;
    h9bus_connector->shutdown_read();
    if (recv_thread_desc.joinable())
        recv_thread_desc.join();

    delete h9bus_connector;
}

void Bus::load_config(DCtx *ctx) {
    send_timeout = ctx->cfg_h9bus_send_frame_timeout();
    Node::recv_timeout = ctx->cfg_h9bus_recv_response_timeout();

    h9bus_connector = new H9Connector(ctx->cfg_h9bus_hostname(), std::to_string(ctx->cfg_h9bus_port()));

    try {
        h9bus_connector->connect("h9d");
        h9bus_connector->send(ExecuteMethodMsg("subscribe").add_value("event", "frame"));
        //h9bus_connector->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    }
    catch (std::system_error &e) {
        h9_log_crit("Can not connect to h9bus %s:%hu: %s", ctx->cfg_h9bus_hostname().c_str(), ctx->cfg_h9bus_port(), e.code().message().c_str());
        exit(EXIT_FAILURE);
    }
    catch (std::runtime_error &e) {
        h9_log_crit("Can not connect to h9bus %s:%hu: authentication fail", ctx->cfg_h9bus_hostname().c_str(), ctx->cfg_h9bus_port());
        exit(EXIT_FAILURE);
    }
    recv_thread_desc = std::thread([this]() {
       this->recv_thread();
    });
}

std::uint8_t Bus::get_next_seqnum(std::uint16_t source_id) noexcept {
    static std::atomic<std::uint8_t> next_seqnum[1 << H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH] = {};
    std::uint8_t ret = next_seqnum[source_id]++;
    return ret & ((1 << H9frame::H9FRAME_SEQNUM_BIT_LENGTH) - 1);
}

int Bus::send_set_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::size_t nbyte, const std::uint8_t *data) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::SET_REG;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;

    if (nbyte > 7) return INVALID_DATA_SIZE;
    frame.dlc = nbyte + 1;

    for (int i = 0; i < nbyte; ++i) {
        frame.data[i+1] = data[i];
    }

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_get_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::GET_REG;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;

    frame.dlc = 1;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_set_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::SET_BIT;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;
    frame.data[1] = bit;
    frame.dlc = 2;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_clear_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::CLEAR_BIT;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;
    frame.data[1] = bit;
    frame.dlc = 2;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_toggle_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::TOGGLE_BIT;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;
    frame.data[1] = bit;
    frame.dlc = 2;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_node_upgrade(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::NODE_UPGRADE;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.dlc = 0;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_node_reset(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::NODE_RESET;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    if (destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.dlc = 0;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

int Bus::send_node_discover(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination) noexcept {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::DISCOVER;

    frame.seqnum = seqnum;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.destination_id = destination;

    frame.dlc = 0;

    int ret = send_frame_sync(frame);
    return ret == Bus::OK ? frame.seqnum : ret;
}

void Bus::run() {

}
