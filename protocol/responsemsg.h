/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-22.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_RESPONSEMSG_H
#define H9_RESPONSEMSG_H

#include "common/logger.h"
#include "concretizemsg.h"
#include "genericmethod.h"
#include "value.h"


class ResponseMsg:public GenericMethod<GenericMsg::Type::RESPONSE, ResponseMsg>  {
private:
    Value _result;
public:
    ResponseMsg(GenericMsg&& k);
    ResponseMsg(const std::string& method_name);

    Value& result();
    int errorno() const;
    std::string errormsg() const;
};


#endif //H9_RESPONSEMSG_H
