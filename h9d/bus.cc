/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "bus.h"
#include <thread>
#include "protocol/framemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"


std::uint8_t Bus::get_next_seqnum(std::uint16_t source_id) {
    static std::uint8_t next_seqnum[1 << H9frame::H9FRAME_SOURCE_ID_BIT_LENGTH] = {0};
    std::uint8_t ret = next_seqnum[source_id];
    next_seqnum[source_id] = (next_seqnum[source_id] + 1) & ((1 << H9frame::H9FRAME_SEQNUM_BIT_LENGTH) - 1);
    return ret;
}

void Bus::recv_thread(void) {
    while (true) {
        GenericMsg raw_msg = h9bus_connector->recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME) {
            FrameMsg msg = std::move(raw_msg);
            H9frame frame = msg.get_frame();
            h9_log_info("frame %llu %llu", msg.get_id(), msg.get_request_id());
            if (msg.get_request_id() != 0) {
                send_promise_map_mtx.lock();
                if (send_promise_map.count(msg.get_request_id())) {
                    send_promise_map[msg.get_request_id()].set_value(int(Bus::OK));
                    send_promise_map.erase(0);
                }
                send_promise_map_mtx.unlock();
            }
        }
    }
}

int Bus::send_frame_sync(H9frame frame) {
    std::promise<int> send_promise;
    std::future<int> send_future = send_promise.get_future();
    std::uint64_t msg_id = h9bus_connector->get_next_id();
    send_promise_map_mtx.lock();
    send_promise_map[msg_id] = std::move(send_promise);
    send_promise_map_mtx.unlock();

    h9bus_connector->send(SendFrameMsg(frame), msg_id);
    if (send_future.wait_for(std::chrono::seconds(send_timeout)) != std::future_status::ready) {
        send_promise_map_mtx.lock();
        send_promise_map.erase(msg_id);
        send_promise_map_mtx.unlock();
        return SEND_TIMEOUT;
    }
    return send_future.get();
}

Bus::Bus(void): h9bus_connector(nullptr) {
    h9_log_info("cons %d", std::this_thread::get_id() );
}

Bus::~Bus(void) {
    h9_log_info("des %d", std::this_thread::get_id() );
    if (recv_thread_desc.joinable())
        recv_thread_desc.join();

    delete h9bus_connector;
}

void Bus::load_config(DCtx *ctx) {
    h9bus_connector = new H9Connector(ctx->cfg_h9bus_hostname(), std::to_string(ctx->cfg_h9bus_port()));

    h9bus_connector->connect();
    h9bus_connector->send(SubscribeMsg(SubscribeMsg::Content::FRAME));

    recv_thread_desc = std::thread([this]() {
       this->recv_thread();
    });
}

int Bus::set_reg(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::size_t nbyte, void *data) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::SET_REG;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;

    if (nbyte > 7) return INVALID_DATA_SIZE;
    frame.dlc = nbyte + 1;

    for (int i = 0; i < nbyte; ++i) {
        frame.data[i+1] = static_cast<uint8_t*>(data)[i];
    }

    return send_frame_sync(frame);
}

int Bus::get_reg(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination, std::uint8_t reg) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::GET_REG;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;

    frame.dlc = 1;

    return send_frame_sync(frame);
}

int Bus::set_bit(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::SET_BIT;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;
    frame.data[1] = bit;
    frame.dlc = 2;

    return send_frame_sync(frame);
}

int Bus::clear_bit(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::CLEAR_BIT;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;
    frame.data[1] = bit;
    frame.dlc = 2;

    return send_frame_sync(frame);
}

int Bus::toggle_bit(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::TOGGLE_BIT;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.data[0] = reg;
    frame.data[1] = bit;
    frame.dlc = 2;

    return send_frame_sync(frame);
}

int Bus::node_upgrade(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::NODE_UPGRADE;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination < 1 || destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.dlc = 0;

    return send_frame_sync(frame);
}

int Bus::node_reset(H9frame::Priority priority, std::uint16_t source, std::uint16_t destination) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::NODE_RESET;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    if (destination > H9frame::BROADCAST_ID) return INVALID_DESTINATION_ID;
    frame.destination_id = destination;

    frame.dlc = 0;

    return send_frame_sync(frame);
}

int Bus::node_discovery(H9frame::Priority priority, std::uint16_t source) {
    H9frame frame;
    frame.priority = priority;
    frame.type = H9frame::Type::DISCOVERY;

    if (source < 1 || source >= H9frame::BROADCAST_ID) return INVALID_SOURCE_ID;
    frame.source_id = source;

    frame.seqnum = get_next_seqnum(source);

    frame.destination_id = H9frame::BROADCAST_ID;

    frame.dlc = 0;

    return send_frame_sync(frame);
}
