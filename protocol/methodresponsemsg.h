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
    MethodResponseMsg(const std::string& method_name, bool execute_fail = false);

    bool get_execute_fail();
    bool get_execute_ok();

    Value& result();
    int errorno() const;
    std::string errormsg() const;
};


#endif //H9_METHODRESPONSEMSG_H
