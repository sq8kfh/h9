/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "framereceivedmsg.h"

FrameReceivedMsg::FrameReceivedMsg(GenericMsg&& k): FrameMsg(std::move(k)) {
}

FrameReceivedMsg::FrameReceivedMsg(const H9frame& frame): FrameMsg(frame) {
}
