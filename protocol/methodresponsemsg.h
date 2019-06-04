/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-22.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */
#ifndef H9_METHODRESPONSEMSG_H
#define H9_METHODRESPONSEMSG_H

#include "common/logger.h"
#include "concretizemsg.h"
#include "genericmethod.h"


class MethodResponseMsg:public GenericMethod<GenericMsg::Type::METHODRESPONSE, MethodResponseMsg>  {
public:
    MethodResponseMsg(GenericMsg&& k);
    MethodResponseMsg(const std::string& method_name);
};


#endif //H9_METHODRESPONSEMSG_H
