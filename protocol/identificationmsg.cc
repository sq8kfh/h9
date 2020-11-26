/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "identificationmsg.h"


IdentificationMsg::IdentificationMsg(GenericMsg&& k): ConcretizeMsg(std::move(k)) {

}

IdentificationMsg::IdentificationMsg(const std::string& entity): ConcretizeMsg<GenericMsg::Type::IDENTIFICATION>() {
    xmlNodePtr root = get_msg_root();

    xmlNewProp(root, reinterpret_cast<xmlChar const *>("entity"), reinterpret_cast<xmlChar const *>(entity.c_str()));
}

std::string IdentificationMsg::get_entity() {
    xmlNodePtr msg = get_msg_root();
    xmlChar *tmp;
    if ((tmp = xmlGetProp(msg, (const xmlChar *) "entity")) == nullptr) {
        throw GenericMsg::InvalidMsg("missing 'entity' property");
    }
    std::string ret = {reinterpret_cast<char const *>(tmp)};
    xmlFree(tmp);
    return ret;
}
