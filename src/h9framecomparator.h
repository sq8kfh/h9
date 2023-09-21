/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-10.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include "ext_h9frame.h"

class H9FrameComparator {
  private:
    constexpr static std::uint16_t PRIORITY = 1;
    constexpr static std::uint16_t SOURCE_ID = 2;
    constexpr static std::uint16_t SEQNUM = 4;
    constexpr static std::uint16_t TYPE_SET1 = 8;
    constexpr static std::uint16_t DESTINATION_ID_SET1 = 16;
    constexpr static std::uint16_t DATA_SET1 = 32;
    constexpr static std::uint16_t FIRST_DATA_BYTE_SET1 = 64;
    constexpr static std::uint16_t TYPE_SET2 = 128;
    constexpr static std::uint16_t DESTINATION_ID_SET2 = 256;
    constexpr static std::uint16_t DATA_SET2 = 512;
    constexpr static std::uint16_t FIRST_DATA_BYTE_SET2 = 1024;
    constexpr static std::uint16_t SEQNUM_OVERRIDE = 2048;

    std::uint16_t fields_to_compare;

    H9frame::Priority priority;
    std::uint16_t source_id;
    std::uint8_t seqnum;
    std::uint8_t seqnum_override;
    H9frame::Type type_set1;
    std::uint16_t destination_id_set1;
    std::uint8_t dlc_set1;
    std::uint8_t data_set1[8];
    std::uint8_t first_data_byte_set1;
    H9frame::Type type_set2;
    std::uint16_t destination_id_set2;
    std::uint8_t dlc_set2;
    std::uint8_t data_set2[8];
    std::uint8_t first_data_byte_set2;

    bool eq(const ExtH9Frame& b) const;
    bool eq_alternate_set(const ExtH9Frame& b) const;
  public:
    H9FrameComparator();
    explicit H9FrameComparator(std::uint16_t source_id_v);
    bool operator==(const ExtH9Frame& b) const;
    bool operator<(const H9FrameComparator& b) const;

    void set_priority(H9frame::Priority priority_v);
    void set_seqnum(std::uint8_t seqnum_v);
    void set_source_id(std::uint16_t source_id_v);

    void seqnum_override_for_alternate_set(std::uint8_t seqnum_v);

    void set_type(H9frame::Type type_v);
    void set_type_in_alternate_set(H9frame::Type type_v);

    void set_destination_id(std::uint16_t destination_id_v);
    void set_destination_id_in_alternate_set(std::uint16_t destination_id_v);

    void set_data(const std::uint8_t* data_v, std::size_t nbyte);
    void set_data_in_alternate_set(const std::uint8_t* data_v, std::size_t nbyte);

    void set_first_data_byte(std::uint8_t data0);
    void set_first_data_byte_in_alternate_set(std::uint8_t data0);
};
