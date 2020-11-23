/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-23.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVICE_H
#define H9_DEVICE_H

#include "config.h"
#include "node.h"


class DevMgr;

class Device: public Node {
private:
    const std::uint16_t node_type;
    const std::uint16_t node_version;

    friend class DevMgr;
    void update_device_state(H9frame frame);
public:
    Device(Bus* bus, std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version) noexcept;

    std::uint16_t get_node_type() noexcept;
    std::uint16_t get_node_version() noexcept;
};


#endif //H9_DEVICE_H
