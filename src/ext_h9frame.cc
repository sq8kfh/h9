/*
 * H9 project
 *
 * Created by crowx on 2023-09-07.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "ext_h9frame.h"

ExtH9Frame::ExtH9Frame(const H9frame& frame, const std::string& origin):
    _frame(frame),
    _origin(origin) {
}

void to_json(nlohmann::json& j, const ExtH9Frame& f) {
    std::vector<std::uint8_t> data(f.data(), f.data() + f.dlc());
    j = nlohmann::json{{"origin", f.origin()},
                       {"priority", f.priority() == H9frame::Priority::HIGH ? "H" : "L"},
                       {"type", H9frame::to_underlying(f.type())},
                       {"seqnum", f.seqnum()},
                       {"destination_id", f.destination_id()},
                       {"source_id", f.source_id()},
                       {"dlc", f.dlc()},
                       {"data", data}};
}

void from_json(const nlohmann::json& j, ExtH9Frame& f) {
    f.origin(j.at("origin").get<std::string>());
    if (j.at("priority").get<std::string>() == "H") {
        f.priority(H9frame::Priority::HIGH);
    }
    else {
        f.priority(H9frame::Priority::LOW);
    }
    f.type(j.at("type").get<std::uint8_t>());
    f.seqnum(j.at("seqnum").get<std::uint8_t>());
    f.destination_id(j.at("destination_id").get<std::uint16_t>());
    f.source_id(j.at("source_id").get<std::uint16_t>());
    f.dlc(j.at("dlc").get<std::uint8_t>());
    f.data(j.at("data").get<std::vector<std::uint8_t>>());
}
