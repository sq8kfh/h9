/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_CALLMSG_H_
#define _H9_CALLMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"
#include "genericmethod.h"
#include "value.h"


class CallMsg: public GenericMethod<GenericMsg::Type::CALL, CallMsg> {
public:
    CallMsg(GenericMsg&& k);
    CallMsg(const std::string& method_name);
};


#endif //_H9_METHODCALLMSG_H_
