/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "devicemethodresponsemsg.h"


DeviceMethodResponseMsg::DeviceMethodResponseMsg(GenericMsg&& k): GenericMethod(std::move(k)) {
}

DeviceMethodResponseMsg::DeviceMethodResponseMsg(std::uint16_t id, const std::string& method_name, bool execute_fail): GenericMethod(method_name) {
    xmlNodePtr root = get_msg_root();

    constexpr int str_sizie = 12;
    char str[str_sizie];
    std::snprintf(str, str_sizie, "%hu", id);
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("id"), reinterpret_cast<xmlChar const *>(str));

    if (execute_fail) {
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("execute_status"), reinterpret_cast<xmlChar const *>("FAIL"));
    }
    else {
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("execute_status"), reinterpret_cast<xmlChar const *>("OK"));
    }
}

bool DeviceMethodResponseMsg::get_execute_fail() {
    xmlNodePtr msg = get_msg_root();
    xmlChar *tmp;
    if ((tmp = xmlGetProp(msg, (const xmlChar *) "execute_status")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'execute_status' property");
    }
    std::string execute_status = {reinterpret_cast<char const *>(tmp)};
    xmlFree(tmp);
    return execute_status == "FAIL";
}

bool DeviceMethodResponseMsg::get_execute_ok() {
    return !get_execute_fail();
}

std::uint16_t DeviceMethodResponseMsg::get_device_id() {
    xmlNodePtr root = get_msg_root();
    xmlChar *tmp;

    if ((tmp = xmlGetProp(root, (const xmlChar *) "id")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'id' property");
    }
    std::uint16_t ret = (uint8_t)strtol((char *)tmp, (char **)nullptr, 10);
    xmlFree(tmp);
    return ret;
}
