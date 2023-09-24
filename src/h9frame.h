/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <cstdint>
#include <ostream>

struct H9frame {
    enum class Priority : std::uint8_t { HIGH = 0,
                                         LOW = 1 };
    enum class Type : std::uint8_t {
        NOP = 0,
        PAGE_START = 1,
        QUIT_BOOTLOADER = 2,
        PAGE_FILL = 3,
        BOOTLOADER_TURNED_ON = 4,
        PAGE_FILL_NEXT = 5,
        PAGE_WRITED = 6,
        PAGE_FILL_BREAK = 7,
        SET_REG = 8,
        GET_REG = 9,
        SET_BIT = 10,
        CLEAR_BIT = 11,
        TOGGLE_BIT = 12,
        NODE_UPGRADE = 13,
        NODE_RESET = 14,
        DISCOVER = 15,
        REG_EXTERNALLY_CHANGED = 16,
        REG_INTERNALLY_CHANGED = 17,
        REG_VALUE_BROADCAST = 18,
        REG_VALUE = 19,
        ERROR = 20,
        NODE_HEARTBEAT = 21,
        NODE_INFO = 22,
        NODE_TURNED_ON = 23,
        NODE_SPECIFIC_BULK0 = 24,
        NODE_SPECIFIC_BULK1 = 25,
        NODE_SPECIFIC_BULK2 = 26,
        NODE_SPECIFIC_BULK3 = 27,
        NODE_SPECIFIC_BULK4 = 28,
        NODE_SPECIFIC_BULK5 = 29,
        NODE_SPECIFIC_BULK6 = 30,
        NODE_SPECIFIC_BULK7 = 31
    };

    enum class Error : std::uint8_t {
        INVALID_MSG = 1,
        UNSUPPORTED_BOOTLOADER = 2,
        INVALID_REGISTER = 3,
        NODE_SPECIFIC_ERROR = 0xff,
    };

    constexpr static int H9FRAME_PRIORITY_BIT_LENGTH = 1;
    constexpr static int H9FRAME_TYPE_BIT_LENGTH = 5;
    constexpr static int H9FRAME_SEQNUM_BIT_LENGTH = 5;
    constexpr static int H9FRAME_DESTINATION_ID_BIT_LENGTH = 9;
    constexpr static int H9FRAME_SOURCE_ID_BIT_LENGTH = 9;

    constexpr static std::uint16_t BROADCAST_ID = 0x01ff;

    constexpr static int H9FRAME_PRIORITY_MAX_VALUE = (1 << H9FRAME_PRIORITY_BIT_LENGTH) - 1;
    constexpr static int H9FRAME_TYPE_MAX_VALUE = (1 << H9FRAME_TYPE_BIT_LENGTH) - 1;
    constexpr static int H9FRAME_SEQNUM_MAX_VALUE = (1 << H9FRAME_SEQNUM_BIT_LENGTH) - 1;
    constexpr static int H9FRAME_DESTINATION_ID_MAX_VALUE = BROADCAST_ID;
    constexpr static int H9FRAME_SOURCE_ID_MAX_VALUE = BROADCAST_ID - 1;
    constexpr static int H9FRAME_DATA_LENGTH = 8;

    Priority priority;
    Type type;
    std::uint8_t seqnum: H9FRAME_SEQNUM_BIT_LENGTH;
    std::uint16_t destination_id: H9FRAME_DESTINATION_ID_BIT_LENGTH;
    std::uint16_t source_id: H9FRAME_SOURCE_ID_BIT_LENGTH;
    std::uint8_t dlc;
    std::uint8_t data[H9FRAME_DATA_LENGTH]{};

    H9frame();
    void set_type_from_underlying(std::underlying_type_t<Type> int_type);

    /*template <typename E>
    static std::underlying_type_t<E> to_underlying(E e) noexcept {
        return static_cast<std::underlying_type_t<E>>(e);
    }*/

    template<typename E, typename R = std::underlying_type_t<E>>
    static R to_underlying(E e) noexcept {
        return static_cast<R>(e);
    }

    template<typename E>
    static E from_underlying(std::underlying_type_t<E> e) noexcept {
        return static_cast<E>(e);
    }

    static const char* type_to_string(Type type);
    static const char* error_to_string(Error error);
};

std::ostream& operator<<(std::ostream& os, const H9frame& frame);
