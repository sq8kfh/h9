/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-09.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_ANTENNASWITCH_H
#define H9_ANTENNASWITCH_H

#include "config.h"
#include "h9d/device.h"

class AntennaSwitch: public Device {
public:
    AntennaSwitch(Bus* bus, std::uint16_t node_id, std::uint16_t node_version) noexcept;
    std::vector<std::string> get_device_specific_methods() noexcept;
    H9Value execute_device_specific_method(const std::string &method_name, const H9Tuple& tuple);
};


#endif //H9_ANTENNASWITCH_H
