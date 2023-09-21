/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-10.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "h9framecomparator.h"

H9FrameComparator::H9FrameComparator():
    fields_to_compare(0),
    priority(H9frame::Priority::LOW),
    source_id(0),
    seqnum(0),
    seqnum_override(0),
    type_set1(H9frame::Type::NOP),
    type_set2(H9frame::Type::NOP),
    destination_id_set1(0),
    destination_id_set2(0),
    dlc_set1(0),
    dlc_set2(0),
    data_set1{},
    data_set2{},
    first_data_byte_set1(0),
    first_data_byte_set2(0) {
}

H9FrameComparator::H9FrameComparator(std::uint16_t source_id_v):
    H9FrameComparator() {
    set_source_id(source_id_v);
}

bool H9FrameComparator::eq(const ExtH9Frame& b) const {
    if (fields_to_compare & H9FrameComparator::SEQNUM && seqnum != b.seqnum())
        return false;

    if (fields_to_compare & H9FrameComparator::TYPE_SET1 && type_set1 != b.type())
        return false;

    if (fields_to_compare & H9FrameComparator::DESTINATION_ID_SET1 && destination_id_set1 != b.destination_id())
        return false;

    if (fields_to_compare & H9FrameComparator::DATA_SET1) {
        if (dlc_set1 != b.dlc())
            return false;
        else {
            for (int i = 0; i < dlc_set1; ++i) {
                if (data_set1[i] != b.data()[i])
                    return false;
            }
        }
    }
    if (fields_to_compare & H9FrameComparator::FIRST_DATA_BYTE_SET1) {
        if (b.dlc() < 1 || first_data_byte_set1 != b.data()[0])
            return false;
    }
    return true;
}

bool H9FrameComparator::eq_alternate_set(const ExtH9Frame& b) const {
    if (fields_to_compare & H9FrameComparator::SEQNUM_OVERRIDE) {
        if (seqnum_override != b.seqnum())
            return false;
    }
    else if (fields_to_compare & H9FrameComparator::SEQNUM && seqnum != b.seqnum())
        return false;

    if (fields_to_compare & H9FrameComparator::TYPE_SET2 && type_set2 != b.type())
        return false;

    if (fields_to_compare & H9FrameComparator::DESTINATION_ID_SET2 && destination_id_set2 != b.destination_id())
        return false;

    if (fields_to_compare & H9FrameComparator::DATA_SET2) {
        if (dlc_set2 != b.dlc())
            return false;
        else {
            for (int i = 0; i < dlc_set2; ++i) {
                if (data_set2[i] != b.data()[i])
                    return false;
            }
        }
    }
    if (fields_to_compare & H9FrameComparator::FIRST_DATA_BYTE_SET2) {
        if (b.dlc() < 1 || first_data_byte_set2 != b.data()[0])
            return false;
    }
    return fields_to_compare & (H9FrameComparator::TYPE_SET2 | H9FrameComparator::DESTINATION_ID_SET2 | H9FrameComparator::DATA_SET2 | H9FrameComparator::FIRST_DATA_BYTE_SET2);
}

bool H9FrameComparator::operator==(const ExtH9Frame& b) const {
    if (fields_to_compare & H9FrameComparator::PRIORITY && priority != b.priority())
        return false;
    if (fields_to_compare & H9FrameComparator::SOURCE_ID && source_id != b.source_id())
        return false;


    return eq(b) || eq_alternate_set(b);
}

bool H9FrameComparator::operator<(const H9FrameComparator& b) const {
    if (fields_to_compare < b.fields_to_compare)
        return true;
    if (priority < b.priority)
        return true;
    if (source_id < b.source_id)
        return true;

    if (seqnum < b.seqnum)
        return true;

    if (type_set1 < b.type_set1)
        return true;
    if (destination_id_set1 < b.destination_id_set1)
        return true;
    if (dlc_set1 < b.dlc_set1)
        return true;
    else if (dlc_set1 == b.dlc_set1) {
        for (int i = 0; i < dlc_set1; ++i) {
            if (data_set1[i] < b.data_set1[i])
                return true;
        }
    }
    if (first_data_byte_set1 < b.first_data_byte_set1)
        return true;

    if (seqnum_override < b.seqnum_override)
        return true;

    if (type_set2 < b.type_set2)
        return true;
    if (destination_id_set2 < b.destination_id_set2)
        return true;
    if (dlc_set2 < b.dlc_set2)
        return true;
    else if (dlc_set2 == b.dlc_set2) {
        for (int i = 0; i < dlc_set2; ++i) {
            if (data_set2[i] < b.data_set2[i])
                return true;
        }
    }
    if (first_data_byte_set2 < b.first_data_byte_set2)
        return true;

    return false;
}

void H9FrameComparator::set_priority(H9frame::Priority priority_v) {
    priority = priority_v;
    fields_to_compare |= H9FrameComparator::PRIORITY;
}

void H9FrameComparator::set_seqnum(std::uint8_t seqnum_v) {
    seqnum = seqnum_v;
    fields_to_compare |= H9FrameComparator::SEQNUM;
}

void H9FrameComparator::seqnum_override_for_alternate_set(std::uint8_t seqnum_v) {
    seqnum_override = seqnum_v;
    fields_to_compare |= H9FrameComparator::SEQNUM_OVERRIDE;
}

void H9FrameComparator::set_source_id(std::uint16_t source_id_v) {
    source_id = source_id_v;
    fields_to_compare |= H9FrameComparator::SOURCE_ID;
}

void H9FrameComparator::set_type(H9frame::Type type_v) {
    type_set1 = type_v;
    fields_to_compare |= H9FrameComparator::TYPE_SET1;
}

void H9FrameComparator::set_type_in_alternate_set(H9frame::Type type_v) {
    type_set2 = type_v;
    fields_to_compare |= H9FrameComparator::TYPE_SET2;
}

void H9FrameComparator::set_destination_id(std::uint16_t destination_id_v) {
    destination_id_set1 = destination_id_v;
    fields_to_compare |= H9FrameComparator::DESTINATION_ID_SET1;
}

void H9FrameComparator::set_destination_id_in_alternate_set(std::uint16_t destination_id_v) {
    destination_id_set2 = destination_id_v;
    fields_to_compare |= H9FrameComparator::DESTINATION_ID_SET2;
}

void H9FrameComparator::set_data(const std::uint8_t* data_v, std::size_t nbyte) {
    for (std::size_t i = 0; i < nbyte; ++i) {
        data_set1[i] = data_v[i];
    }
    dlc_set1 = nbyte;
    fields_to_compare |= H9FrameComparator::DATA_SET1;
}

void H9FrameComparator::set_data_in_alternate_set(const std::uint8_t* data_v, std::size_t nbyte) {
    for (std::size_t i = 0; i < nbyte; ++i) {
        data_set2[i] = data_v[i];
    }
    dlc_set2 = nbyte;
    fields_to_compare |= H9FrameComparator::DATA_SET2;
}

void H9FrameComparator::set_first_data_byte(std::uint8_t data0) {
    first_data_byte_set1 = data0;
    fields_to_compare |= H9FrameComparator::FIRST_DATA_BYTE_SET1;
}

void H9FrameComparator::set_first_data_byte_in_alternate_set(std::uint8_t data0) {
    first_data_byte_set2 = data0;
    fields_to_compare |= H9FrameComparator::FIRST_DATA_BYTE_SET2;
}
