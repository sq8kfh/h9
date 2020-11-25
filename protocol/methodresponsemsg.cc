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

MethodResponseMsg::MethodResponseMsg(const std::string& method_name, bool execute_fail): GenericMethod(method_name), _result(get_msg_root()) {
    xmlNodePtr root = get_msg_root();
    if (execute_fail) {
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("execute_status"), reinterpret_cast<xmlChar const *>("FAIL"));
    }
    else {
        xmlNewProp(root, reinterpret_cast<xmlChar const *>("execute_status"), reinterpret_cast<xmlChar const *>("OK"));
    }
}

bool MethodResponseMsg::get_execute_fail() {
    xmlNodePtr msg = get_msg_root();
    xmlChar *tmp;
    if ((tmp = xmlGetProp(msg, (const xmlChar *) "execute_status")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'execute_status' property");
    }
    std::string execute_status = {reinterpret_cast<char const *>(tmp)};
    xmlFree(tmp);
    return execute_status == "FAIL";
}

bool MethodResponseMsg::get_execute_ok() {
    return !get_execute_fail();
}

Value& MethodResponseMsg::result() {
    return _result;
}

int MethodResponseMsg::errorno() const {
    return 0;
}

std::string MethodResponseMsg::errormsg() const {
    return "";
}
