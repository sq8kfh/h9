/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_SUBSCRIBEMSG_H_
#define _H9_SUBSCRIBEMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"


class SubscribeMsg: public ConcretizeMsg<GenericMsg::Type::SUBSCRIBE> {
public:
    enum class Content {
        NONE = 0,
        FRAME,
    };
    SubscribeMsg(GenericMsg&& k);
    SubscribeMsg(Content content);
    Content get_content();
};


#endif //_H9_SUBSCRIBEMSG_H_
