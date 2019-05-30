/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "methodcallmsg.h"


MethodCallMsg::MethodCallMsg(GenericMsg&& k): GenericMethod(std::move(k)) {

}

MethodCallMsg::MethodCallMsg(const std::string& method_name): GenericMethod(method_name) {

}
