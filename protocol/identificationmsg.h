/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-24.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_IDENTIFICATIONMSG_H
#define H9_IDENTIFICATIONMSG_H

#include "config.h"
#include "concretizemsg.h"


class IdentificationMsg: public ConcretizeMsg<GenericMsg::Type::IDENTIFICATION>{
public:
    IdentificationMsg(GenericMsg&& k);
    IdentificationMsg(const std::string& entity);

    std::string get_entity();
};


#endif //H9_IDENTIFICATIONMSG_H
