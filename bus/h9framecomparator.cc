/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-10.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "h9framecomparator.h"

H9FrameComparator::H9FrameComparator(void):
    fields_to_compare(0),
    priority(H9frame::Priority::LOW),
    type(H9frame::Type::NOP),
    alternate_type(H9frame::Type::NOP),
    seqnum(0),
    destination_id(0),
    source_id(0),
    dlc(0),
    data{},
    first_data_byte(0) {

}

H9FrameComparator::H9FrameComparator(std::uint16_t source_id_v): H9FrameComparator() {
    set_source_id(source_id_v);
}

bool H9FrameComparator::operator==(const H9frame &b) const {
    if (fields_to_compare & H9FrameComparator::PRIORITY && priority != b.priority) return false;
    if (fields_to_compare & H9FrameComparator::TYPE && type != b.type &&
        (!(fields_to_compare & H9FrameComparator::ALTERNATE_TYPE) || (alternate_type != b.type))) return false;
    if (fields_to_compare & H9FrameComparator::SEQNUM && seqnum != b.seqnum) return false;
    if (fields_to_compare & H9FrameComparator::DESTINATION_ID && destination_id != b.destination_id) return false;
    if (fields_to_compare & H9FrameComparator::SOURCE_ID && source_id != b.source_id) return false;
    if (fields_to_compare & H9FrameComparator::DATA) {
        if (dlc != b.dlc) return false;
        else {
            for (int i = 0; i < dlc; ++i) {
                if (data[i] != b.data[i]) return false;
            }
        }
    }
    if (fields_to_compare & H9FrameComparator::FIRST_DATA_BYTE) {
        if (b.dlc < 1) return false;
        else if (first_data_byte != b.data[0]) return false;
    }
    return true;
}

bool H9FrameComparator::operator<(const H9FrameComparator &b) const {
    if (fields_to_compare < b.fields_to_compare) return true;
    if (priority < b.priority) return true;
    if (type < b.type) return true;
    if (seqnum < b.seqnum) return true;
    if (destination_id < b.destination_id) return true;
    if (source_id < b.source_id) return true;
    if (dlc < b.dlc) return true;
    else if (dlc == b.dlc) {
        for (int i = 0; i < dlc; ++i) {
            if (data[i] < b.data[i]) return true;
        }
    }
    if (first_data_byte < b.first_data_byte) return true;
    return false;
}

void H9FrameComparator::set_priority(H9frame::Priority priority_v) {
    priority = priority_v;
    fields_to_compare |= H9FrameComparator::PRIORITY;
}

void H9FrameComparator::set_type(H9frame::Type type_v) {
    type = type_v;
    fields_to_compare |= H9FrameComparator::TYPE;
}

void H9FrameComparator::set_alternate_type(H9frame::Type type_v) {
    alternate_type = type_v;
    fields_to_compare |= H9FrameComparator::ALTERNATE_TYPE;
}

void H9FrameComparator::set_seqnum(std::uint8_t seqnum_v) {
    seqnum = seqnum_v;
    fields_to_compare |= H9FrameComparator::SEQNUM;
}

void H9FrameComparator::set_destination_id(std::uint16_t destination_id_v) {
    destination_id = destination_id_v;
    fields_to_compare |= H9FrameComparator::DESTINATION_ID;
}

void H9FrameComparator::set_source_id(std::uint16_t source_id_v) {
    source_id = source_id_v;
    fields_to_compare |= H9FrameComparator::SOURCE_ID;
}

void H9FrameComparator::set_data(std::uint8_t *data_v, std::size_t nbyte) {
    for (std::size_t i = 0; i < nbyte; ++i) {
        data[i] = data_v[i];
    }
    dlc = nbyte;
    fields_to_compare |= H9FrameComparator::DATA;
}

void H9FrameComparator::set_first_data_byte(std::uint8_t data0) {
    first_data_byte = data0;
    fields_to_compare |= H9FrameComparator::FIRST_DATA_BYTE;
}
