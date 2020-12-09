/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-12-09.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "antennaswitch.h"

AntennaSwitch::AntennaSwitch(Bus* bus, std::uint16_t node_id, std::uint16_t node_version) noexcept:
    Device(bus, node_id, 5, node_version) {

}
