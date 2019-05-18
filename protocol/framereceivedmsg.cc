#include "framereceivedmsg.h"

FrameReceivedMsg::FrameReceivedMsg(GenericMsg&& k): FrameMsg(std::move(k)) {
}

FrameReceivedMsg::FrameReceivedMsg(const H9frame& frame): FrameMsg(frame) {
}
