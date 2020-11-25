/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EXECUTEDEVICEMETHODMSG_H
#define H9_EXECUTEDEVICEMETHODMSG_H

#include "config.h"
#include "genericmethod.h"
#include "value.h"


class ExecuteDeviceMethodMsg: public GenericMethod<GenericMsg::Type::DEVICEMETHODRESPONSE, ExecuteDeviceMethodMsg> {
public:
    ExecuteDeviceMethodMsg(GenericMsg&& k);
    ExecuteDeviceMethodMsg(std::uint16_t id, const std::string& method_name);

    std::uint16_t get_id();
};


#endif //H9_EXECUTEDEVICEMETHODMSG_H
