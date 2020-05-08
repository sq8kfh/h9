/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_FRAMEMSG_H_
#define _H9_FRAMEMSG_H_

#include <string>
#include "genericframemsg.h"
#include "bus/h9frame.h"


class FrameMsg: public GenericFrameMsg<GenericMsg::Type::FRAME> {
public:
    FrameMsg(GenericMsg&& k);
    FrameMsg(const H9frame& frame, const std::string& origin, const std::string& endpoint = "");

    std::string get_origin();
};


#endif //_H9_FRAMEMSG_H_
