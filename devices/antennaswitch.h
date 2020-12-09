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
};


#endif //H9_ANTENNASWITCH_H
