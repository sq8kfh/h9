/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "executedevicemethodmsg.h"


ExecuteDeviceMethodMsg::ExecuteDeviceMethodMsg(GenericMsg&& k): GenericMethod(std::move(k)) {

}

ExecuteDeviceMethodMsg::ExecuteDeviceMethodMsg(std::uint16_t id, const std::string& method_name): GenericMethod(method_name) {
    xmlNodePtr root = get_msg_root();

    constexpr int str_sizie = 12;
    char str[str_sizie];
    std::snprintf(str, str_sizie, "%hu", id);
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("id"), reinterpret_cast<xmlChar const *>(str));
}

std::uint16_t ExecuteDeviceMethodMsg::get_id() {
    xmlNodePtr root = get_msg_root();
    xmlChar *tmp;

    if ((tmp = xmlGetProp(root, (const xmlChar *) "id")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'id' property");
    }
    std::uint16_t ret = (uint8_t)strtol((char *)tmp, (char **)nullptr, 10);
    xmlFree(tmp);
    return ret;
}