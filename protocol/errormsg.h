/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_ERRORMSG_H_
#define _H9_ERRORMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"


class ErrorMsg: public ConcretizeMsg<GenericMsg::Type::ERROR> {
public:
    ErrorMsg(GenericMsg&& k);
    ErrorMsg(int errnum, std::string msg);
};


#endif //_H9_ERRORMSG_H_
