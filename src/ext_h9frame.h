/*
 * H9 project
 *
 * Created by crowx on 2023-09-07.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EXT_H9FRAME_H
#define H9_EXT_H9FRAME_H

#include "h9frame.h"
#include <string>

class ExtH9Frame {
private:
    H9frame _frame;
    std::string _origin;
public:
    ExtH9Frame() = default;
    ExtH9Frame(const H9frame& frame, const std::string& origin);

    const H9frame& frame() const { return _frame; }

    const std::string& origin() const { return _origin; }
    void origin(const std::string& origin) { _origin = origin; }

    H9frame::Priority priority() const { return _frame.priority; }
    void priority(H9frame::Priority priority) { _frame.priority = priority; }

    H9frame::Type type() const { return  _frame.type; }
    void type(H9frame::Type type) { _frame.type = type; }

    std::uint8_t seqnum() const { return _frame.seqnum; }
    void seqnum(std::uint8_t seqnum) { _frame.seqnum = seqnum; }

    std::uint16_t destination_id() const { return _frame.destination_id; }
    void destination_id(std::uint16_t destination_id) { _frame.destination_id = destination_id; }

    std::uint16_t source_id() const { return _frame.source_id; }
    void source_id(std::uint16_t source_id) { _frame.source_id = source_id; }

    std::uint8_t dlc() const { return _frame.dlc; }
    void dlc(std::uint8_t dlc) { _frame.dlc = dlc; }

    const std::uint8_t* data() const { return _frame.data; };
    void data(const std::vector<std::uint8_t>& data) {
        int i = 0;
        for (auto &d : data) {
            _frame.data[i] = d;
            ++i;
            if (i > 8) break;
        }
    }
};


#endif //H9_EXT_H9FRAME_H
