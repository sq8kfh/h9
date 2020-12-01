/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-22.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "methodresponsemsg.h"


MethodResponseMsg::MethodResponseMsg(GenericMsg&& k): GenericMethod(std::move(k)), _result(get_msg_root()) {

}

MethodResponseMsg::MethodResponseMsg(const std::string& method_name): GenericMethod(method_name), _result(get_msg_root()) {

}

MethodResponseMsg::MethodResponseMsg(const std::string& method_name, unsigned int error_code, const std::string& error_message): GenericMethod(method_name), _result(get_msg_root()) {
    xmlNodePtr error_node = xmlNewNode(nullptr, reinterpret_cast<xmlChar const *>("error"));

    xmlSetProp(error_node, reinterpret_cast<xmlChar const *>("code"), reinterpret_cast<xmlChar const *>(std::to_string(error_code).c_str()));
    xmlNewProp(error_node, reinterpret_cast<xmlChar const *>("message"), reinterpret_cast<xmlChar const *>(error_message.c_str()));

    xmlAddChild(get_msg_root(), error_node);
}

unsigned int MethodResponseMsg::get_error_code() {
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

std::string MethodResponseMsg::get_error_message() {
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

Value& MethodResponseMsg::result() {
    return _result;
}
