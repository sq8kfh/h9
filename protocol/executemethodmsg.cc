/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "executemethodmsg.h"


ExecuteMethodMsg::ExecuteMethodMsg(GenericMsg&& k): GenericMethod(std::move(k)) {

}

ExecuteMethodMsg::ExecuteMethodMsg(const std::string& method_name): GenericMethod(method_name) {

}
