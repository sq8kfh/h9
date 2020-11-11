/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "node.h"
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

Node::Node(Bus *bus, std::uint16_t node_id): BusObserver(bus, H9FrameComparator(node_id)), node_id(node_id) {
}

Node::~Node(void) {
    h9_log_info("~Node");
}

int Node::reset(void) {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::NODE_TURNED_ON);
    auto future = create_frame_future(comparator);
    h9bus->send_node_reset(H9frame::Priority::LOW, source_id, node_id);

    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    future.get();
    return 0;
}

int Node::get_node_type(void) {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::REG_VALUE);
    //comparator.set_alternate_type(H9frame::Type::ERROR);
    comparator.set_first_data_byte(1);

    auto future = create_frame_future(comparator);
    h9bus->send_get_reg(H9frame::Priority::LOW, source_id, node_id, 1);
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

    auto future = create_frame_future(comparator);
    h9bus->send_get_reg(H9frame::Priority::LOW, source_id, node_id, 2);
    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    H9frame res = future.get();
    if (major) *major = res.data[1];
    if (minor) *minor = res.data[2];
    return res.data[1] << 8 | res.data[2];
}

int Node::set_node_id(std::uint16_t id) {
    H9FrameComparator comparator;
    comparator.set_source_id(node_id);
    comparator.set_type(H9frame::Type::REG_EXTERNALLY_CHANGED);
    comparator.set_first_data_byte(3);

    auto future = create_frame_future(comparator);
    h9bus->send_set_reg(H9frame::Priority::LOW, source_id, node_id, 3, id);
    if (future.wait_for(std::chrono::seconds(timeout)) != std::future_status::ready) {
        return -1;
    }
    future.get();

    return h9bus->send_node_reset(H9frame::Priority::LOW, source_id, node_id);
}
