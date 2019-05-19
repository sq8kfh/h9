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
    enum class ErrorNumber {
        INVALID_MESSAGE_SCHEMA,
        UNSUPPORTED_MESSAGE_TYPE
    };
    ErrorMsg(GenericMsg&& k);
    ErrorMsg(ErrorNumber errnum, const std::string& msg = "");
    ErrorNumber get_errnum();
    std::string get_msg();

    static const char* errnum_to_string(ErrorNumber errnum);
};


#endif //_H9_ERRORMSG_H_
