/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_NODE_H
#define H9_NODE_H

#include "config.h"
#include <chrono>
#include <list>
#include <future>
#include <map>
#include <tuple>
#include "bus.h"
#include "frameobserver.h"


class Node: public FrameObserver {
private:
    Bus* const h9bus;

    std::mutex frame_promise_list_mtx;
    std::list<std::tuple<H9FrameComparator, std::promise<H9frame>, std::chrono::time_point<std::chrono::steady_clock>>> frame_promise_list;

    void on_frame_recv(H9frame frame) final;
    std::future<H9frame> create_frame_future(H9FrameComparator comparator);

    std::uint16_t source_id = 1;
    int timeout = 5;
protected:
    const std::uint16_t node_id;
public:
    Node(Bus* bus, std::uint16_t node_id);
    ~Node();

    std::uint16_t get_node_id() noexcept;

    int reset();
    int get_node_type();
    int get_node_version(std::uint8_t *major = nullptr, std::uint8_t *minor = nullptr);
    int set_node_id(std::uint16_t id);

    int set_reg(std::uint8_t reg, std::uint8_t value);
    //int get_reg(std::uint8_t reg, );
};


#endif //H9_NODE_H
