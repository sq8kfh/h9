/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "device.h"
#include "common/logger.h"


void Device::update_device_state(H9frame frame) {

}

Device::Device(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept: Node(bus, node_id), node_type(node_type), node_version(node_version) {
    h9_log_info("Create device descriptor: id: %hu type: %hu version: %hhu.%hhu", node_id, node_type, node_version >> 8, node_version);
}

std::uint16_t Device::get_node_type() noexcept {
    return node_type;
}

std::uint16_t Device::get_node_version() noexcept {
    return node_version;
}
