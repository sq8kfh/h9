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
    ++received_frames_counter;
    recv_frame_callback(this, frame);
}

void Driver::on_frame_send() {
    ++sent_frames_counter;
    BusFrame *tmp = send_queue.top();
    send_queue.pop();
    send_frame_callback(this, tmp);
    if (!send_queue.empty()) {
        send_data(send_queue.top()->get_frame());
    }
}

Driver::Driver(const std::string& name, TRecvFrameCallback recv_frame_callback, TSendFrameCallback send_frame_callback):
        name(name),
        recv_frame_callback(std::move(recv_frame_callback)),
        send_frame_callback(std::move(send_frame_callback)),
        sent_frames_counter(0),
        received_frames_counter(0) {

    retry_auto_connect = 10;
}

void Driver::on_close() noexcept {
    ::close(get_socket());
    disconnected();
}

std::uint32_t Driver::get_counter(BusMgr::CounterType counter) {
    switch (counter) {
        case BusMgr::CounterType::SEND_FRAMES:
            return sent_frames_counter;
        case BusMgr::CounterType::RECEIVED_FRAMES:
            return received_frames_counter;
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
