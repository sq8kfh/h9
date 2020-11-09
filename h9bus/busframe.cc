/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-05-08.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "busframe.h"


BusFrame::BusFrame(const H9frame& frame, const std::string& origin, std::uint64_t orgin_client_id, std::uint64_t orgin_msg_id):
frame(frame), origin(origin), orgin_client_id(orgin_client_id), orgin_msg_id(orgin_msg_id) {
    total_endpoint_count = 0;
    completed_endpoint_count = 0;
}

const H9frame& BusFrame::get_frame() const {
    return frame;
}

const std::string& BusFrame::get_origin() const {
    return origin;
}

std::uint64_t BusFrame::get_orgin_client_id(void) const {
    return orgin_client_id;
}

std::uint64_t BusFrame::get_orgin_msg_id(void) const {
    return orgin_msg_id;
}

unsigned int BusFrame::inc_total_endpoint_count() {
    return ++total_endpoint_count;
}

unsigned int BusFrame::get_total_endpoint_count() const {
    return total_endpoint_count;
}

unsigned int BusFrame::int_completed_endpoint_count() {
    return ++completed_endpoint_count;
}

unsigned int BusFrame::get_completed_endpoint_count() const {
    return completed_endpoint_count;
}