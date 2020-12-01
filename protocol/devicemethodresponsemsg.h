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
private:
    Value _result;
public:
    DeviceMethodResponseMsg(GenericMsg&& k);
    DeviceMethodResponseMsg(std::uint16_t device_id, const std::string& method_name);
    DeviceMethodResponseMsg(std::uint16_t device_id, const std::string& method_name, unsigned int error_code, const std::string& error_message);

    unsigned int get_error_code();
    std::string get_error_message();

    std::uint16_t get_device_id();

    Value& result();
};


#endif //H9_DEVICEMETHODRESPONSEMSG_H
