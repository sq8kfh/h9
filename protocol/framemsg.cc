/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "framemsg.h"

FrameMsg::FrameMsg(GenericMsg&& k): GenericFrameMsg(std::move(k)) {
}

FrameMsg::FrameMsg(const H9frame& frame, const std::string& origin, const std::string& endpoint): GenericFrameMsg(frame, endpoint) {
    xmlNodePtr root = get_msg_root();
    xmlNewProp(root, reinterpret_cast<xmlChar const *>("origin"), reinterpret_cast<xmlChar const *>(origin.c_str()));
}

std::string FrameMsg::get_origin() {
    xmlNodePtr root = get_msg_root();

    xmlChar *tmp;

    if ((tmp = xmlGetProp(root, (const xmlChar *) "origin")) == nullptr) {
        h9_log_err("Frame: missing 'origin' property");
        throw GenericMsg::InvalidMsg("missing 'origin' property");
    }
    std::string ret = std::string(reinterpret_cast<const char *>(tmp));
    xmlFree(tmp);

    return ret;
}