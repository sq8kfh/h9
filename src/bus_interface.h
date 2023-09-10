/*
 * H9 project
 *
 * Created by crowx on 2023-09-07.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "busframe.h"

class BusInterface {
  public:
    // PAGE_START = 1,
    // QUIT_BOOTLOADER = 2,
    // PAGE_FILL = 3,
    // BOOTLOADER_TURNED_ON = 4,
    // PAGE_FILL_NEXT = 5,
    // PAGE_WRITED = 6,
    // PAGE_FILL_BREAK = 7,
    int send_set_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, std::size_t nbyte, const std::uint8_t* data) noexcept;
    int send_get_reg(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg) noexcept;
    int send_set_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) noexcept;
    int send_clear_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) noexcept;
    int send_toggle_bit(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination, std::uint8_t reg, int bit) noexcept;
    int send_node_upgrade(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination) noexcept;
    int send_node_reset(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination) noexcept;
    int send_node_discover(H9frame::Priority priority, std::uint8_t seqnum, std::uint16_t source, std::uint16_t destination = H9frame::BROADCAST_ID) noexcept;
    // REG_EXTERNALLY_CHANGED = 16,
    // REG_INTERNALLY_CHANGED = 17,
    // REG_VALUE_BROADCAST = 18,
    // REG_VALUE = 19,
    // ERROR = 20,
    // NODE_HEARTBEAT = 21,
    // NODE_INFO = 22,
    // NODE_TURNED_ON = 23,
    // NODE_SPECIFIC_BULK0 = 24,
    // NODE_SPECIFIC_BULK1 = 25,
    // NODE_SPECIFIC_BULK2 = 26,
    // NODE_SPECIFIC_BULK3 = 27,
    // NODE_SPECIFIC_BULK4 = 28,
    // NODE_SPECIFIC_BULK5 = 29,
    // NODE_SPECIFIC_BULK6 = 30,
    // NODE_SPECIFIC_BULK7 = 31
};
