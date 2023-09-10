/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-10.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_H9FRAMECOMPARATOR_H
#define H9_H9FRAMECOMPARATOR_H

#include "config.h"

#include "ext_h9frame.h"

class H9FrameComparator {
  private:
    constexpr static std::uint8_t PRIORITY = 1;
    constexpr static std::uint8_t TYPE = 2;
    constexpr static std::uint8_t ALTERNATE_TYPE = 4;
    constexpr static std::uint8_t SEQNUM = 8;
    constexpr static std::uint8_t DESTINATION_ID = 16;
    constexpr static std::uint8_t SOURCE_ID = 32;
    constexpr static std::uint8_t DATA = 64;
    constexpr static std::uint8_t FIRST_DATA_BYTE = 128;
    std::uint8_t fields_to_compare;

    H9frame::Priority priority;
    H9frame::Type type;
    H9frame::Type alternate_type;
    std::uint8_t seqnum;
    std::uint16_t destination_id;
    std::uint16_t source_id;
    std::uint8_t dlc;
    std::uint8_t data[8];
    std::uint8_t first_data_byte;

  public:
    H9FrameComparator(void);
    H9FrameComparator(std::uint16_t source_id_v);
    bool operator==(const ExtH9Frame& b) const;
    bool operator<(const H9FrameComparator& b) const;
    void set_priority(H9frame::Priority priority_v);
    void set_type(H9frame::Type type_v);
    void set_alternate_type(H9frame::Type type_v);
    void set_seqnum(std::uint8_t seqnum_v);
    void set_destination_id(std::uint16_t destination_id_v);
    void set_source_id(std::uint16_t source_id_v);
    void set_data(std::uint8_t* data_v, std::size_t nbyte);
    void set_first_data_byte(std::uint8_t data0);
};

#endif // H9_H9FRAMECOMPARATOR_H
