/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include <utility>

#include "driver.h"


void Driver::on_frame_recv(const H9frame& frame) {
    _event_callback.on_fame_recv(frame);
}

void Driver::on_frame_send(const H9frame& frame) {
    send_queue.pop();
    _event_callback.on_fame_send(frame);
    if (send_queue.size() > 0) {
        send_data(send_queue.front());
    }
}

Driver::Driver(BusMgr::EventCallback event_callback):
        _event_callback(std::move(event_callback)),
        next_seqnum(0) {

    retry_auto_connect = 10;
}

void Driver::close() {
    int socket = get_socket();
    ::close(socket);
    _event_callback.on_close();
    set_socket(0);
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
    if (get_socket() > 0) {
        ::close(get_socket());
        set_socket(0);
    }
}
