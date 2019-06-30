/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-29.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "expression.h"

const char* NodeExp::completion_list[] = {
        "reg",
        "restart",
        nullptr
};

const char* NodeRegExp::completion_list[] = {
        "get",
        "set",
        nullptr
};
