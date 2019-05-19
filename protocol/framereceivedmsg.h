/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_FRAMERECEIVEDMSG_H_
#define _H9_FRAMERECEIVEDMSG_H_

#include "framemsg.h"
#include "bus/h9frame.h"


class FrameReceivedMsg: public FrameMsg<GenericMsg::Type::FRAME_RECEIVED> {
public:
    FrameReceivedMsg(GenericMsg&& k);
    FrameReceivedMsg(const H9frame& frame);
};


#endif //_H9_FRAMERECEIVEDMSG_H_
