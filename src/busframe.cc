/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-05-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "busframe.h"

BusFrame::BusFrame(): _local_frame(false) {
    _number_of_active_bus = 0;
    _send_counter = 0;
    _send_fail_counter = 0;
    _activate_promise = false;
}

BusFrame::BusFrame(const H9frame& frame, const std::string& origin, std::uint64_t orgin_client_id, std::uint64_t orgin_msg_id):
    _local_frame(false),
    ExtH9Frame(frame, origin),
    orgin_client_id(orgin_client_id),
    orgin_msg_id(orgin_msg_id) {
    _number_of_active_bus = 0;
    _send_counter = 0;
    _send_fail_counter = 0;
    _activate_promise = false;

    //SPDLOG_TRACE("BusFrame id: {}; origin: {}", fmt::ptr(this), this->origin());
}

BusFrame::BusFrame(ExtH9Frame&& a, bool raw) noexcept:
    _local_frame(false),
    ExtH9Frame(std::move(a)),
    _raw(raw) {
    _number_of_active_bus = 0;
    _send_counter = 0;
    _send_fail_counter = 0;
    _activate_promise = false;

    //SPDLOG_TRACE("BusFrame id: {}; origin: {}", fmt::ptr(this), this->origin());
}

BusFrame::~BusFrame() {
    //SPDLOG_TRACE("~BusFrame id: {}; origin: {}, cr: {} incr: {}", fmt::ptr(this), origin(), _send_counter, _send_fail_counter);
}

bool BusFrame::raw() const {
    return _raw;
}

bool BusFrame::local_origin_frame() const {
    return _local_frame;
}

void BusFrame::mark_as_local_origin() {
    _local_frame = true;
}

std::uint64_t BusFrame::get_orgin_client_id() const {
    return orgin_client_id;
}

std::uint64_t BusFrame::get_orgin_msg_id() const {
    return orgin_msg_id;
}

std::promise<SendFrameResult>& BusFrame::get_send_promise() {
    return _send_promise;
}

void BusFrame::activate_send_finish_promise(unsigned int active_buses) {
    _activate_promise = true;
    _number_of_active_bus = active_buses;
}

bool BusFrame::is_sent_finish() const {
    return (_send_counter + _send_fail_counter) >= _number_of_active_bus;
}

void BusFrame::inc_send_counter() {
    ++_send_counter;
    if (_activate_promise && is_sent_finish()) {
        _send_promise.set_value({seqnum(), source_id(), _send_counter, _send_fail_counter});
    }
}

void BusFrame::inc_send_fail_counter() {
    ++_send_fail_counter;
    if (_activate_promise && is_sent_finish()) {
        _send_promise.set_value({seqnum(), source_id(), _send_counter, _send_fail_counter});
    }
}

