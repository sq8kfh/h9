/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "driver.h"

#include <utility>
#include <cstring>


void Driver::on_frame_recv(const H9frame& frame) {
    ++received_frames;
    _event_callback.on_fame_recv(frame);
}

void Driver::on_frame_send(const H9frame& frame) {
    ++sent_frames;
    send_queue.pop();
    _event_callback.on_fame_send(frame);
    if (send_queue.size() > 0) {
        send_data(send_queue.front());
    }
}

Driver::Driver(BusMgr::EventCallback event_callback):
        _event_callback(std::move(event_callback)),
        next_seqnum(0),
        sent_frames(0),
        received_frames(0) {

    retry_auto_connect = 10;
}

void Driver::on_close() noexcept {
    ::close(get_socket());
    disconnected();
    //_event_callback.on_close();
    //set_socket(0);
}

std::uint32_t Driver::get_counter(BusMgr::CounterType counter) {
    switch (counter) {
        case BusMgr::CounterType::SEND_FRAMES:
            return sent_frames;
        case BusMgr::CounterType::RECEIVED_FRAMES:
            return received_frames;
    }
    return 0;
}

void Driver::send_frame(const H9frame& frame) {
    if (get_socket() == 0) {
        return;
    }
    bool queue_empty = send_queue.empty();
    send_queue.push(frame);
    send_queue.back().seqnum = next_seqnum;

    ++next_seqnum;
    next_seqnum &= ((1<<H9frame::H9FRAME_SEQNUM_BIT_LENGTH)-1);

    if (queue_empty) {
        send_data(send_queue.front());
    }
}

void Driver::on_select() {
    recv_data();
}

Driver::~Driver() {
}
