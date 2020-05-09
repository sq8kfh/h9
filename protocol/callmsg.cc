/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "callmsg.h"


CallMsg::CallMsg(GenericMsg&& k): GenericMethod(std::move(k)) {

}

CallMsg::CallMsg(const std::string& method_name): GenericMethod(method_name) {

}
