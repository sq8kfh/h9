/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-26.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVICEEVENT_H
#define H9_DEVICEEVENT_H

#include "config.h"
#include "concretizemsg.h"
#include "common/logger.h"
#include "value.h"


class DeviceEvent: public ConcretizeMsg<GenericMsg::Type::DEVICEEVENT> {
private:
    DictValue parres;
public:
    DeviceEvent(GenericMsg&& k);
    DeviceEvent(std::uint16_t device_id, const std::string& event_name);

    std::uint16_t get_device_id();
    std::string get_event_name();

    ArrayValue add_array(const char* name);
    DictValue add_dict(const char* name);

    template<typename value_t>
    DeviceEvent& add_value(const std::string &name, value_t value) {
        parres.add_value(name, value);
        return *this;
    }

    Value operator[](const char* name);
};


#endif //H9_DEVICEEVENT_H
