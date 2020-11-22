/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_BUS_H
#define H9_BUS_H

#include "config.h"
#include <atomic>
#include <thread>
#include <future>
#include <list>
#include <map>
#include "bus/h9frame.h"
#include "bus/h9framecomparator.h"
#include "protocol/h9connector.h"
#include "busobserver.h"
#include "connectionctx.h"
#include "dctx.h"

class Bus {
public:
    constexpr static int OK = 0;
    constexpr static int NOT_CONNECTED_TO_H9BUS = -1;
    constexpr static int INVALID_SOURCE_ID = -2;
    constexpr static int INVALID_DESTINATION_ID = -3;
    constexpr static int INVALID_DATA_SIZE = -4;
    constexpr static int SEND_TIMEOUT = -5;
    constexpr static int H9BUS_ERROR = -6;
private:
    H9Connector *h9bus_connector;
    std::thread recv_thread_desc;

    std::mutex send_promise_map_mtx;
    std::map<std::uint64_t, std::promise<int>> send_promise_map;

    std::mutex frame_observers_mtx;
    std::map<H9FrameComparator, std::list<BusObserver*>> frame_observers;
    void attach_frame_observer(BusObserver *observer, H9FrameComparator comparator);
    void detach_frame_observer(BusObserver *observer);
    void notify_observer(const H9frame &frame);
    friend BusObserver::BusObserver(Bus *bus, H9FrameComparator comparator);
    friend BusObserver::~BusObserver();

    void recv_thread();
    int send_frame_sync(H9frame frame);

    int send_timeout = 5;
    std::atomic_bool run = {true};
public:
    Bus();
    Bus(const Bus &a) = delete;
    ~Bus();
    void load_config(DCtx *ctx);
    std::uint8_t get_next_seqnum(std::uint16_t source_id);

    //PAGE_START = 1,
    //QUIT_BOOTLOADER = 2,
    //PAGE_FILL = 3,
    //BOOTLOADER_TURNED_ON = 4,
    //PAGE_FILL_NEXT = 5,
    //PAGE_WRITED = 6,
    //PAGE_FILL_BREAK = 7,
    int send_set_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::size_t nbyte, void *data);
    int send_set_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::uint8_t reg_value);
    int send_set_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::uint16_t reg_value);
    int send_set_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::uint32_t reg_value);
    int send_get_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg);
    int send_set_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit);
    int send_clear_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit);
    int send_toggle_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit);
    int send_node_upgrade(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination);
    int send_node_reset(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination);
    int send_node_discover(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source);
    //REG_EXTERNALLY_CHANGED = 16,
    //REG_INTERNALLY_CHANGED = 17,
    //REG_VALUE_BROADCAST = 18,
    //REG_VALUE = 19,
    //ERROR = 20,
    //NODE_HEARTBEAT = 21,
    //NODE_INFO = 22,
    //NODE_TURNED_ON = 23,
    //NODE_SPECIFIC_BULK0 = 24,
    //NODE_SPECIFIC_BULK1 = 25,
    //NODE_SPECIFIC_BULK2 = 26,
    //NODE_SPECIFIC_BULK3 = 27,
    //NODE_SPECIFIC_BULK4 = 28,
    //NODE_SPECIFIC_BULK5 = 29,
    //NODE_SPECIFIC_BULK6 = 30,
    //NODE_SPECIFIC_BULK7 = 31
};


#endif //H9_BUS_H
