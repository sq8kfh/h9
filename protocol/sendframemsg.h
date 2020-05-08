/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_SENDFRAMEMSG_H_
#define _H9_SENDFRAMEMSG_H_

#include "genericframemsg.h"
#include "bus/h9frame.h"


class SendFrameMsg: public GenericFrameMsg<GenericMsg::Type::SEND_FRAME> {
public:
    SendFrameMsg(GenericMsg&& k);
    SendFrameMsg(const H9frame& frame);
};


#endif //_H9_SENDFRAMEMSG_H_
