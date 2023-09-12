/*
 * H9 project
 *
 * Created by crowx on 2023-09-07.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "h9frame.h"

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

    H9frame::Type type() const { return _frame.type; }

    void type(H9frame::Type type) { _frame.type = type; }
    void type(std::uint8_t type) { _frame.set_type_from_underlying(type); }

    std::uint8_t seqnum() const { return _frame.seqnum; }

    void seqnum(std::uint8_t seqnum) { _frame.seqnum = seqnum; }

    std::uint16_t destination_id() const { return _frame.destination_id; }

    void destination_id(std::uint16_t destination_id) { _frame.destination_id = destination_id; }

    std::uint16_t source_id() const { return _frame.source_id; }

    void source_id(std::uint16_t source_id) { _frame.source_id = source_id; }

    std::uint8_t dlc() const { return _frame.dlc; }

    void dlc(std::uint8_t dlc) { _frame.dlc = dlc <= 8 ? dlc : 8; }

    const std::uint8_t* data() const { return _frame.data; };

    void data(const std::vector<std::uint8_t>& data) {
        int i = 0;
        for (auto& d : data) {
            _frame.data[i] = d;
            ++i;
            if (i > 8)
                break;
        }
    }
};

void to_json(nlohmann::json& j, const ExtH9Frame& f);
void from_json(const nlohmann::json& j, ExtH9Frame& f);
