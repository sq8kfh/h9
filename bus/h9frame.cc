/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "h9frame.h"

#include <iomanip>
#include <cassert>

H9frame::H9frame(): priority(Priority::LOW), type(Type::NOP), dlc(0) {
}

void H9frame::set_type_from_underlying(std::underlying_type_t<H9frame::Type> int_type) {
    assert(int_type < (2<<H9FRAME_TYPE_BIT_LENGTH));
    type = H9frame::from_underlying<H9frame::Type>(int_type);
}

std::ostream& operator<<(std::ostream& os, const H9frame& frame) {
    os << frame.source_id << " -> " << frame.destination_id
    << " priority: " << (frame.priority == H9frame::Priority::HIGH ? 'H' : 'L')
    << " type: " << static_cast<unsigned int>(frame.to_underlying(frame.type))
    << " seqnum: " << static_cast<unsigned int>(frame.seqnum)
    << " dlc: " << static_cast<unsigned int>(frame.dlc)
    << " data:";
    std::ios oldState(nullptr);
    oldState.copyfmt(os);

    for (int i = 0; i < frame.dlc; ++i) {
        os << ' ' << std::setfill('0') << std::hex << std::setw(2) << static_cast<unsigned int>(frame.data[i]);
    }
    os.copyfmt(oldState);
    return os;
}
