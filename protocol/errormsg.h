/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_ERRORMSG_H_
#define _H9_ERRORMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"


class ErrorMsg: public ConcretizeMsg<GenericMsg::Type::ERROR> {
public:
    enum class ErrorNumber {
        INVALID_MESSAGE_SCHEMA,
        UNSUPPORTED_MESSAGE_TYPE,
        UNSUPPORTED_METHOD,
        INVALID_PARAMETERS,
        INVALID_DEVICE,
        UNSUPPORTED_DEVICE_METHOD,
        UNSUPPORTED_EVENT,
    };
    ErrorMsg(GenericMsg&& k);
    ErrorMsg(ErrorNumber errnum, const std::string& msg = "");
    ErrorNumber get_errnum();
    std::string get_msg();

    static const char* errnum_to_string(ErrorNumber errnum);
};


#endif //_H9_ERRORMSG_H_
