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

    void on_frame_recv(H9frame frame) noexcept final;
    std::future<H9frame> create_frame_future(H9FrameComparator comparator) noexcept;

    std::uint16_t source_id = 1;

    friend void Bus::load_config(DCtx *ctx);
    static int recv_timeout;
protected:
    const std::uint16_t node_id;
public:
    Node(Bus* bus, std::uint16_t node_id) noexcept;
    ~Node() noexcept;

    std::uint16_t get_node_id() noexcept;

    int reset() noexcept;
    int get_node_type() noexcept;
    int get_node_version(std::uint8_t *major = nullptr, std::uint8_t *minor = nullptr) noexcept;
    int set_node_id(std::uint16_t id) noexcept;

    ssize_t set_raw_reg(std::uint8_t reg, std::size_t nbyte, const std::uint8_t *buf, std::uint8_t *setted = nullptr) noexcept;
    ssize_t set_raw_reg(std::uint8_t reg, std::uint8_t value, std::uint8_t *setted = nullptr) noexcept;
    ssize_t set_raw_reg(std::uint8_t reg, std::uint16_t value, std::uint16_t *setted = nullptr) noexcept;
    ssize_t set_raw_reg(std::uint8_t reg, std::uint32_t value, std::uint32_t *setted = nullptr) noexcept;
    ssize_t get_raw_reg(std::uint8_t reg, std::size_t nbyte, std::uint8_t *buf) noexcept;
};


#endif //H9_NODE_H
