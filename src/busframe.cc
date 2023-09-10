/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-05-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "busframe.h"

BusFrame::BusFrame() {
    _number_of_active_bus = 0;
    _send_counter = 0;
}

BusFrame::BusFrame(const H9frame& frame, const std::string& origin, std::uint64_t orgin_client_id, std::uint64_t orgin_msg_id):
    ExtH9Frame(frame, origin),
    orgin_client_id(orgin_client_id),
    orgin_msg_id(orgin_msg_id) {
    _number_of_active_bus = 0;
    _send_counter = 0;
}

BusFrame::BusFrame(ExtH9Frame&& a) noexcept:
    ExtH9Frame(std::move(a)) {
    _number_of_active_bus = 0;
    _send_counter = 0;
}

std::uint64_t BusFrame::get_orgin_client_id(void) const {
    return orgin_client_id;
}

std::uint64_t BusFrame::get_orgin_msg_id(void) const {
    return orgin_msg_id;
}

std::promise<int>& BusFrame::get_send_promise() {
    return _send_promise;
}

void BusFrame::set_number_of_active_bus(unsigned int v) {
    _number_of_active_bus = v;
}

bool BusFrame::is_sent_finish() {
    return _send_counter >= _number_of_active_bus;
}

void BusFrame::inc_send_counter() {
    ++_send_counter;
    if (_number_of_active_bus && is_sent_finish()) {
        _send_promise.set_value(_send_counter);
    }
}
