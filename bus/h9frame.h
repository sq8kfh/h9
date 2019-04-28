#ifndef _H9_H9FRAME_H_
#define _H9_H9FRAME_H_

#include <cstdint>

class H9frame {
public:
    enum class Priority : std::uint8_t { HIGH = 0, LOW = 1 };
    enum class Type : std::uint8_t {
        NOP = 0,
        PAGE_START = 1,
        QUIT_BOOTLOADER = 2,
        PAGE_FILL = 3,
        ENTER_INTO_BOOTLOADER = 4,
        PAGE_FILL_NEXT = 5,
        PAGE_WRITED = 6,
        PAGE_FILL_BREAK = 7,
        REG_EXTERNALLY_CHANGED = 8,
        REG_INTERNALLY_CHANGED = 9,
        REG_VALUE_BROADCAST = 10,
        REG_VALUE = 11,
        NODE_HEARTBEAT = 12,
        NODE_ERROR = 13,
        U14 = 14,
        U15 = 15,
        SET_REG = 16,
        GET_REG = 17,
        NODE_INFO = 18,
        NODE_RESET = 19,
        NODE_UPGRADE = 20,
        U21 = 21,
        U22 = 22,
        U23 = 23,
        DISCOVERY = 24,
        NODE_TURNED_ON = 25,
        POWER_OFF = 26,
        U27 = 27,
        U29 = 29,
        U30 = 30,
        U28 = 28,
        U31 = 31
    };

    constexpr static std::uint16_t BROADCAST_ID = 0x01ff;
public:
    Priority priority;
    Type type;
    std::uint8_t seqnum;
    std::uint16_t destination_id;
    std::uint16_t source_id;
    std::uint8_t dlc;
    std::uint8_t data[8];
};


#endif //_H9_H9FRAME_H_
