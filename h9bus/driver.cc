/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "driver.h"

#include <utility>
#include <cstring>


void Driver::on_frame_recv(const H9frame& frame) {
    ++received_frames;
    _event_callback.on_fame_recv(frame);
}

void Driver::on_frame_send() {
    ++sent_frames;
    BusFrame *tmp = send_queue.top();
    send_queue.pop();
    _event_callback.on_fame_send(tmp);
    if (!send_queue.empty()) {
        send_data(send_queue.top()->get_frame());
    }
}

Driver::Driver(BusMgr::EventCallback event_callback):
        _event_callback(std::move(event_callback)),
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

void Driver::send_frame(BusFrame *busframe) {
    if (get_socket() == 0) {
        return;
    }
    bool queue_empty = send_queue.empty();
    send_queue.push(busframe);

    if (queue_empty) {
        send_data(send_queue.top()->get_frame());
    }
}

void Driver::on_select() {
    recv_data();
}

Driver::~Driver() {
}
