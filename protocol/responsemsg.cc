/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-22.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "responsemsg.h"


ResponseMsg::ResponseMsg(GenericMsg&& k): GenericMethod(std::move(k)), _result(get_msg_root()) {

}

ResponseMsg::ResponseMsg(const std::string& method_name): GenericMethod(method_name), _result(get_msg_root()) {
}

Value& ResponseMsg::result() {
    return _result;
}

int ResponseMsg::errorno() const {
    return 0;
}

std::string ResponseMsg::errormsg() const {
    return "";
}
