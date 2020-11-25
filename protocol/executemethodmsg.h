/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_EXECUTEMETHODMSG_H_
#define _H9_EXECUTEMETHODMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"
#include "genericmethod.h"
#include "value.h"


class ExecuteMethodMsg: public GenericMethod<GenericMsg::Type::EXECUTEMETHOD, ExecuteMethodMsg> {
public:
    ExecuteMethodMsg(GenericMsg&& k);
    ExecuteMethodMsg(const std::string& method_name);
};


#endif //_H9_EXECUTEMETHODMSG_H_
