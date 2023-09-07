/*
 * H9 project
 *
 * Created by crowx on 2023-09-07.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "ext_h9frame.h"

ExtH9Frame::ExtH9Frame(const H9frame& frame, const std::string& origin):
    _frame(frame),
    _origin(origin) {

}