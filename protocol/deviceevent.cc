/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-26.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "deviceevent.h"


DeviceEvent::DeviceEvent(GenericMsg&& k): ConcretizeMsg(std::move(k)), parres(ConcretizeMsg::get_msg_root()) {

}

DeviceEvent::DeviceEvent(std::uint16_t device_id, const std::string& event_name): ConcretizeMsg(), parres(ConcretizeMsg::get_msg_root()) {
    xmlNodePtr root = get_msg_root();

    constexpr int str_sizie = 12;
    char str[str_sizie];
    std::snprintf(str, str_sizie, "%hu", device_id);
    xmlSetProp(root, reinterpret_cast<xmlChar const *>("device-id"), reinterpret_cast<xmlChar const *>(str));

    xmlNewProp(root, reinterpret_cast<xmlChar const *>("event"), reinterpret_cast<xmlChar const *>(event_name.c_str()));
}

std::uint16_t DeviceEvent::get_device_id() {
    xmlNodePtr root = get_msg_root();
    xmlChar *tmp;

    if ((tmp = xmlGetProp(root, (const xmlChar *) "device-id")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'device-id' property");
    }
    std::uint16_t ret = (uint8_t)strtol((char *)tmp, (char **)nullptr, 10);
    xmlFree(tmp);
    return ret;
}

std::string DeviceEvent::get_event_name() {
    xmlNodePtr msg = get_msg_root();
    xmlChar *tmp;
    if ((tmp = xmlGetProp(msg, (const xmlChar *) "event")) == nullptr) {
        h9_log_err("GenericMethod: missing 'method' property");
        throw GenericMsg::InvalidMsg("missing 'method' property");
    }
    std::string ret = {reinterpret_cast<char const *>(tmp)};
    xmlFree(tmp);
    return ret;
}

ArrayValue DeviceEvent::add_array(const char* name) {
    return parres.add_array(name);
}

DictValue DeviceEvent::add_dict(const char* name) {
    return parres.add_dict(name);
}

Value DeviceEvent::operator[](const char* name) {
    return parres.operator[](name);
}