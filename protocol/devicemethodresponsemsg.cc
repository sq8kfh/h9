/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "devicemethodresponsemsg.h"


DeviceMethodResponseMsg::DeviceMethodResponseMsg(GenericMsg&& k): GenericMethod(std::move(k)), _result(get_msg_root()) {
}

DeviceMethodResponseMsg::DeviceMethodResponseMsg(std::uint16_t device_id, const std::string& method_name): GenericMethod(method_name), _result(get_msg_root()) {
    xmlNodePtr root = get_msg_root();

    xmlSetProp(root, reinterpret_cast<xmlChar const *>("device-id"), reinterpret_cast<xmlChar const *>(std::to_string(device_id).c_str()));
}

DeviceMethodResponseMsg::DeviceMethodResponseMsg(std::uint16_t device_id, const std::string& method_name, unsigned int error_code, const std::string& error_message): GenericMethod(method_name), _result(get_msg_root()) {
    xmlNodePtr root = get_msg_root();
    xmlSetProp(root, reinterpret_cast<xmlChar const *>("device-id"), reinterpret_cast<xmlChar const *>(std::to_string(device_id).c_str()));

    xmlNodePtr error_node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("error"));

    xmlSetProp(error_node, reinterpret_cast<xmlChar const *>("code"), reinterpret_cast<xmlChar const *>(std::to_string(error_code).c_str()));
    xmlNewProp(error_node, reinterpret_cast<xmlChar const *>("message"), reinterpret_cast<xmlChar const *>(error_message.c_str()));

    xmlAddChild(root, error_node);
}

unsigned int DeviceMethodResponseMsg::get_error_code() {
    xmlNodePtr error_node;
    for (error_node = get_msg_root()->children; error_node; error_node = error_node->next) {
        if (error_node->type == XML_ELEMENT_NODE && xmlStrcmp(error_node->name,reinterpret_cast<xmlChar const *>("error")) == 0) {
            break;
        }
    }

    xmlChar *tmp;
    if ((tmp = xmlGetProp(error_node, (const xmlChar *) "code")) == nullptr) {
        return 0;
    }
    int err = std::stoi(reinterpret_cast<char *>(tmp));
    xmlFree(tmp);
    return err;
}

std::string DeviceMethodResponseMsg::get_error_message() {
    xmlNodePtr error_node;
    for (error_node = get_msg_root()->children; error_node; error_node = error_node->next) {
        if (error_node->type == XML_ELEMENT_NODE && xmlStrcmp(error_node->name,reinterpret_cast<xmlChar const *>("error")) == 0) {
            break;
        }
    }

    xmlChar *tmp;
    if ((tmp = xmlGetProp(error_node, (const xmlChar *) "message")) == nullptr) {
        return "";
    }
    std::string ret = {reinterpret_cast<char *>(tmp)};
    xmlFree(tmp);
    return std::move(ret);
}

std::uint16_t DeviceMethodResponseMsg::get_device_id() {
    xmlNodePtr root = get_msg_root();
    xmlChar *tmp;

    if ((tmp = xmlGetProp(root, (const xmlChar *) "device-id")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'device-id' property");
    }
    std::uint16_t ret = (uint16_t)strtol((char *)tmp, (char **)nullptr, 10);
    xmlFree(tmp);
    return ret;
}

Value& DeviceMethodResponseMsg::result() {
    return _result;
}
