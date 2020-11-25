/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVICEMETHODRESPONSEMSG_H
#define H9_DEVICEMETHODRESPONSEMSG_H

#include "config.h"
#include "genericmethod.h"
#include "value.h"


class DeviceMethodResponseMsg: public GenericMethod<GenericMsg::Type::DEVICEMETHODRESPONSE, DeviceMethodResponseMsg> {
public:
    DeviceMethodResponseMsg(GenericMsg&& k);
    DeviceMethodResponseMsg(std::uint16_t id, const std::string& method_name, bool execute_fail = false);

    bool get_execute_fail();
    bool get_execute_ok();

    std::uint16_t get_device_id();
};


#endif //H9_DEVICEMETHODRESPONSEMSG_H
