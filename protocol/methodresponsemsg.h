/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-22.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_METHODRESPONSEMSG_H
#define H9_METHODRESPONSEMSG_H

#include "common/logger.h"
#include "concretizemsg.h"
#include "genericmethod.h"
#include "value.h"


class MethodResponseMsg: public GenericMethod<GenericMsg::Type::METHODRESPONSE, MethodResponseMsg>  {
private:
    Value _result;
public:
    MethodResponseMsg(GenericMsg&& k);
    explicit MethodResponseMsg(const std::string& method_name);
    MethodResponseMsg(const std::string& method_name, unsigned int error_code, const std::string& error_message);

    unsigned int get_error_code();
    std::string get_error_message();

    Value& result();
};


#endif //H9_METHODRESPONSEMSG_H
