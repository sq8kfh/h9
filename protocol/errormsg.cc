/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "errormsg.h"
#include <cassert>
#include <string>


ErrorMsg::ErrorMsg(GenericMsg&& k): ConcretizeMsg(std::move(k)) {
}

ErrorMsg::ErrorMsg(ErrorMsg::ErrorNumber code, const std::string& msg): ConcretizeMsg() {
    xmlNodePtr root = get_msg_root();

    std::string errnum_str = std::to_string(static_cast<int>(code));
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("code"), reinterpret_cast<xmlChar const *>(errnum_str.c_str()));
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("name"), reinterpret_cast<xmlChar const *>(errnum_to_string(code)));
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("message"), reinterpret_cast<xmlChar const *>(msg.c_str()));
}

ErrorMsg::ErrorNumber ErrorMsg::get_code() {
    xmlNodePtr root = get_msg_root();

    xmlChar *tmp;
    if ((tmp = xmlGetProp(root, (const xmlChar *) "code")) == nullptr) {
        h9_log_err("ErrorMsg: missing 'code' property");
        throw GenericMsg::InvalidMsg("missing 'code' property");
    }
    int err = std::stoi(reinterpret_cast<char *>(tmp));
    xmlFree(tmp);
    return static_cast<ErrorMsg::ErrorNumber>(err);
}

std::string ErrorMsg::get_message() {
    xmlNodePtr root = get_msg_root();

    xmlChar *tmp;
    if ((tmp = xmlGetProp(root, (const xmlChar *) "message")) == nullptr) {
        h9_log_warn("ErrorMsg: missing 'message' property");
        throw GenericMsg::InvalidMsg("missing 'message' property");
    }
    std::string ret = {reinterpret_cast<char *>(tmp)};
    xmlFree(tmp);
    return std::move(ret);
}

const char* ErrorMsg::errnum_to_string(ErrorMsg::ErrorNumber errnum) {
    switch (errnum) {
        case ErrorNumber::GENERIC:
            return "GENERIC";
        case ErrorNumber::INVALID_MESSAGE_SCHEMA:
            return "INVALID_MESSAGE_SCHEMA";
        case ErrorNumber::UNSUPPORTED_MESSAGE_TYPE:
            return "UNSUPPORTED_MESSAGE_TYPE";
        case ErrorNumber::UNSUPPORTED_METHOD:
            return "UNSUPPORTED_METHOD";
        case ErrorNumber::INVALID_PARAMETERS:
            return "INVALID_PARAMETERS";
        case ErrorNumber::INVALID_DEVICE:
            return "INVALID_DEVICE";
        case ErrorNumber::UNSUPPORTED_DEVICE_METHOD:
            return "UNSUPPORTED_DEVICE_METHOD";
        case ErrorNumber::UNSUPPORTED_EVENT:
            return "UNSUPPORTED_EVENT";
    }
    assert(false);
}
