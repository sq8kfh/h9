/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-09.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "antennaswitch.h"
#include <cassert>


AntennaSwitch::AntennaSwitch(Bus* bus, std::uint16_t node_id, std::uint16_t node_version) noexcept:
    Device(bus, node_id, 5, node_version) {
}

std::vector<std::string> AntennaSwitch::get_device_specific_methods() noexcept {
    std::vector<std::string> ret;
    ret.emplace_back("select_antenna");
    ret.emplace_back("get_selected_antenna");

    return ret;
}

H9Value AntennaSwitch::execute_device_specific_method(const std::string &method_name, const H9Tuple &tuple) {
    if (method_name == "select_antenna") {

    }
    else if (method_name == "get_selected_antenna") {
        std::int64_t buf;
        ssize_t ret = get_register(10, buf);
        if (ret == 1) {
            return H9Value(buf);
        }
    }
    assert(false);
}
