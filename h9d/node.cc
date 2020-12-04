/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "node.h"
#include <arpa/inet.h>
#include "common/logger.h"
#include "bus.h"


void Node::on_frame_recv(H9frame frame) {
    h9_log_info("on_frame_recv");
    auto now = std::chrono::steady_clock::now();
    frame_promise_list_mtx.lock();
    for (auto it = frame_promise_list.begin(); it != frame_promise_list.end();) {
        if (std::get<0>(*it) == frame) {
            std::get<1>(*it).set_value(frame);
            it = frame_promise_list.erase(it);
        }
        else if (std::get<2>(*it) < now) {
            h9_log_info("timeout");
            //std::get<1>(*it).set_exception(std::make_exception_ptr(1));
            it = frame_promise_list.erase(it);
        }
        else {
            ++it;
        }
    }
    frame_promise_list_mtx.unlock();
}

std::future<H9frame> Node::create_frame_future(H9FrameComparator comparator) {
    std::promise<H9frame> frame_promise;
    std::future<H9frame> frame_future = frame_promise.get_future();

    frame_promise_list_mtx.lock();
    auto t = std::make_tuple(comparator, std::move(frame_promise), std::chrono::steady_clock::now() + std::chrono::seconds(timeout+3));
    frame_promise_list.push_back(std::move(t));
    frame_promise_list_mtx.unlock();

    return frame_future;
}

Node::Node(Bus* bus, std::uint16_t node_id): FrameObserver(bus, H9FrameComparator(node_id)), node_id(node_id), h9bus(bus) {
}

Node::~Node() {
    h9_log_debug2("~Node");
}

std::uint16_t Node::get_node_id() noexcept {
    return node_id;
}

int Node::reset() {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::NODE_TURNED_ON);
    comparator.set_seqnum(0);

    auto future = create_frame_future(comparator);

    auto seqnum = h9bus->get_next_seqnum(node_id);
    h9bus->send_node_reset(H9frame::Priority::LOW, seqnum, source_id, node_id);

    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    future.get();
    return 0;
}

int Node::get_node_type() {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::REG_VALUE);
    //comparator.set_alternate_type(H9frame::Type::ERROR);
    comparator.set_first_data_byte(1);
    //TODO: catch H9frame::Type::ERROR

    auto seqnum = h9bus->get_next_seqnum(node_id);
    comparator.set_seqnum(seqnum);

    auto future = create_frame_future(comparator);
    h9bus->send_get_reg(H9frame::Priority::LOW, seqnum, source_id, node_id, 1);
    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    H9frame res = future.get();
    if (res.type == H9frame::Type::ERROR) return -10;
    return res.data[1] << 8 | res.data[2];
}

int Node::get_node_version(std::uint8_t *major, std::uint8_t *minor) {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::REG_VALUE);
    comparator.set_first_data_byte(2);
    //TODO: catch H9frame::Type::ERROR

    auto seqnum = h9bus->get_next_seqnum(node_id);
    comparator.set_seqnum(seqnum);

    auto future = create_frame_future(comparator);
    h9bus->send_get_reg(H9frame::Priority::LOW, seqnum, source_id, node_id, 2);
    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    H9frame res = future.get();
    if (major) *major = res.data[1];
    if (minor) *minor = res.data[2];
    return res.data[1] << 8 | res.data[2];
}

int Node::set_node_id(std::uint16_t id) {
    if (set_raw_reg(3, id) <= 0) {
        return -1;
    }

    auto seqnum = h9bus->get_next_seqnum(node_id);
    return h9bus->send_node_reset(H9frame::Priority::LOW, seqnum, source_id, node_id);
}

ssize_t Node::set_raw_reg(std::uint8_t reg, std::size_t nbyte, const std::uint8_t *buf, std::uint8_t *setted) {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::REG_EXTERNALLY_CHANGED);
    comparator.set_first_data_byte(reg);
    //TODO: catch H9frame::Type::ERROR

    auto seqnum = h9bus->get_next_seqnum(node_id);
    comparator.set_seqnum(seqnum);

    auto future = create_frame_future(comparator);
    h9bus->send_set_reg(H9frame::Priority::LOW, seqnum, source_id, node_id, reg, nbyte, buf);
    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    H9frame res = future.get();

    if (setted) {
        ssize_t tmp = nbyte < res.dlc - 1 ? nbyte : res.dlc - 1;
        for (int i = 0; i < tmp; ++i) {
            setted[i] = res.data[i+1];
        }
    }

    return res.dlc - 1;
}

ssize_t Node::set_raw_reg(std::uint8_t reg, std::uint8_t value, std::uint8_t *setted) {
    return set_raw_reg(reg, 1, &value, setted);
}

ssize_t Node::set_raw_reg(std::uint8_t reg, std::uint16_t value, std::uint16_t *setted) {
    std::uint16_t bigendian_value = htons(value);
    std::uint16_t tmp;
    ssize_t ret = set_raw_reg(reg, 2, reinterpret_cast<const std::uint8_t*>(&bigendian_value), reinterpret_cast<std::uint8_t*>(&tmp));
    if (setted) *setted = ntohs(tmp);
    return ret;
}

ssize_t Node::set_raw_reg(std::uint8_t reg, std::uint32_t value, std::uint32_t *setted) {
    std::uint32_t bigendian_value = htonl(value);
    std::uint32_t tmp;
    ssize_t ret = set_raw_reg(reg, 4, reinterpret_cast<const std::uint8_t*>(&bigendian_value), reinterpret_cast<std::uint8_t*>(&tmp));
    if (setted) *setted = ntohl(tmp);
    return ret;
}

ssize_t Node::get_raw_reg(std::uint8_t reg, std::size_t nbyte, std::uint8_t *buf) {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::REG_VALUE);
    comparator.set_first_data_byte(reg);
    //TODO: catch H9frame::Type::ERROR

    auto seqnum = h9bus->get_next_seqnum(node_id);
    comparator.set_seqnum(seqnum);

    auto future = create_frame_future(comparator);
    h9bus->send_get_reg(H9frame::Priority::LOW, seqnum, source_id, node_id, reg);
    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    H9frame res = future.get();

    ssize_t ret = nbyte < res.dlc - 1 ? nbyte : res.dlc - 1;

    for (int i = 0; i < ret; ++i) {
        buf[i] = res.data[i+1];
    }

    return ret;
}
