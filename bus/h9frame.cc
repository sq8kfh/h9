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
    << " type: " << std::setw(2) << static_cast<unsigned int>(H9frame::to_underlying(frame.type))
    << " seqnum: " << std::setw(2) << static_cast<unsigned int>(frame.seqnum)
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

const char* H9frame::type_to_string(H9frame::Type type) {
    switch (type) {
        case Type::NOP: return "NOP";
        case Type::PAGE_START: return "PAGE_START";
        case Type::QUIT_BOOTLOADER: return "QUIT_BOOTLOADER";
        case Type::PAGE_FILL: return "PAGE_FILL";
        case Type::ENTER_INTO_BOOTLOADER: return "ENTER_INTO_BOOTLOADER";
        case Type::PAGE_FILL_NEXT: return "PAGE_FILL_NEXT";
        case Type::PAGE_WRITED: return "PAGE_WRITED";
        case Type::PAGE_FILL_BREAK: return "PAGE_FILL_BREAK";
        case Type::REG_EXTERNALLY_CHANGED: return "REG_EXTERNALLY_CHANGED";
        case Type::REG_INTERNALLY_CHANGED: return "REG_INTERNALLY_CHANGED";
        case Type::REG_VALUE_BROADCAST: return "REG_VALUE_BROADCAST";
        case Type::REG_VALUE: return "REG_VALUE";
        case Type::NODE_HEARTBEAT: return "NODE_HEARTBEAT";
        case Type::NODE_ERROR: return "NODE_ERROR";
        case Type::U14: return "U14";
        case Type::U15: return "U15";
        case Type::SET_REG: return "SET_REG";
        case Type::GET_REG: return "GET_REG";
        case Type::NODE_INFO: return "NODE_INFO";
        case Type::NODE_RESET: return "NODE_RESET";
        case Type::NODE_UPGRADE: return "NODE_UPGRADE";
        case Type::U21: return "U21";
        case Type::U22: return "U22";
        case Type::U23: return "U23";
        case Type::DISCOVERY: return "DISCOVERY";
        case Type::NODE_TURNED_ON: return "NODE_TURNED_ON";
        case Type::POWER_OFF: return "POWER_OFF";
        case Type::U27: return "U27";
        case Type::U29: return "U29";
        case Type::U30: return "U30";
        case Type::U28: return "U28";
        case Type::U31: return "U31";
    }
}
