/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-16.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "sendframemsg.h"


SendFrameMsg::SendFrameMsg(GenericMsg&& k): FrameMsg(std::move(k)) {
}

SendFrameMsg::SendFrameMsg(const H9frame& frame): FrameMsg(frame) {
}
