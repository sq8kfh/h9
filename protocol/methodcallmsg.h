/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_METHODCALLMSG_H_
#define _H9_METHODCALLMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"
#include "genericmethod.h"


class MethodCallMsg: public GenericMethod<GenericMsg::Type::METHODCALL, MethodCallMsg> {
public:
    MethodCallMsg(GenericMsg&& k);
    MethodCallMsg(const std::string& method_name);
};


#endif //_H9_METHODCALLMSG_H_
