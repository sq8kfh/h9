/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#include "h9frame.h"

#include <cassert>
#include <iomanip>

H9frame::H9frame():
    priority(Priority::LOW),
    type(Type::NOP),
    dlc(0) {
}

void H9frame::set_type_from_underlying(std::underlying_type_t<H9frame::Type> int_type) {
    assert(int_type < (2 << H9FRAME_TYPE_BIT_LENGTH));
    type = H9frame::from_underlying<H9frame::Type>(int_type);
}

std::ostream& operator<<(std::ostream& os, const H9frame& frame) {
    os << frame.source_id << " -> " << frame.destination_id
       << " priority: " << (frame.priority == H9frame::Priority::HIGH ? 'H' : 'L')
       << " type: " << std::setw(2) << static_cast<unsigned int>(H9frame::to_underlying(frame.type))
       << " (" << H9frame::type_to_string(frame.type) << ")"
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
    case Type::NOP:
        return "NOP";
    case Type::PAGE_START:
        return "PAGE_START";
    case Type::QUIT_BOOTLOADER:
        return "QUIT_BOOTLOADER";
    case Type::PAGE_FILL:
        return "PAGE_FILL";
    case Type::BOOTLOADER_TURNED_ON:
        return "BOOTLOADER_TURNED_ON";
    case Type::PAGE_FILL_NEXT:
        return "PAGE_FILL_NEXT";
    case Type::PAGE_WRITED:
        return "PAGE_WRITED";
    case Type::PAGE_FILL_BREAK:
        return "PAGE_FILL_BREAK";
    case Type::SET_REG:
        return "SET_REG";
    case Type::GET_REG:
        return "GET_REG";
    case Type::SET_BIT:
        return "SET_BIT";
    case Type::CLEAR_BIT:
        return "CLEAR_BIT";
    case Type::TOGGLE_BIT:
        return "TOGGLE_BIT";
    case Type::NODE_UPGRADE:
        return "NODE_UPGRADE";
    case Type::NODE_RESET:
        return "NODE_RESET";
    case Type::DISCOVER:
        return "DISCOVER";
    case Type::REG_EXTERNALLY_CHANGED:
        return "REG_EXTERNALLY_CHANGED";
    case Type::REG_INTERNALLY_CHANGED:
        return "REG_INTERNALLY_CHANGED";
    case Type::REG_VALUE_BROADCAST:
        return "REG_VALUE_BROADCAST";
    case Type::REG_VALUE:
        return "REG_VALUE";
    case Type::ERROR:
        return "ERROR";
    case Type::NODE_HEARTBEAT:
        return "NODE_HEARTBEAT";
    case Type::NODE_INFO:
        return "NODE_INFO";
    case Type::NODE_TURNED_ON:
        return "NODE_TURNED_ON";
    case Type::NODE_SPECIFIC_BULK0:
        return "NODE_SPECIFIC_BULK0";
    case Type::NODE_SPECIFIC_BULK1:
        return "NODE_SPECIFIC_BULK1";
    case Type::NODE_SPECIFIC_BULK2:
        return "NODE_SPECIFIC_BULK2";
    case Type::NODE_SPECIFIC_BULK3:
        return "NODE_SPECIFIC_BULK3";
    case Type::NODE_SPECIFIC_BULK4:
        return "NODE_SPECIFIC_BULK4";
    case Type::NODE_SPECIFIC_BULK5:
        return "NODE_SPECIFIC_BULK5";
    case Type::NODE_SPECIFIC_BULK6:
        return "NODE_SPECIFIC_BULK6";
    case Type::NODE_SPECIFIC_BULK7:
        return "NODE_SPECIFIC_BULK7";
    }
    return nullptr;
}

const char* H9frame::error_to_string(Error error) {
    switch (error) {
    case Error::INVALID_MSG:
        return "INVALID_MSG";
    case Error::UNSUPPORTED_BOOTLOADER:
        return "UNSUPPORTED_BOOTLOADER";
    case Error::INVALID_REGISTER:
        return "INVALID_REGISTER";
    case Error::NODE_SPECIFIC_ERROR:
        return "NODE_SPECIFIC_ERROR";
    }
    return nullptr;
}
