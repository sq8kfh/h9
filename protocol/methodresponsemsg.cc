/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-22.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "methodresponsemsg.h"


MethodResponseMsg::MethodResponseMsg(GenericMsg&& k): GenericMethod(std::move(k)) {

}

MethodResponseMsg::MethodResponseMsg(const std::string& method_name): GenericMethod(method_name) {

}
