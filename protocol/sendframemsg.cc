#include "sendframemsg.h"


SendFrameMsg::SendFrameMsg(GenericMsg&& k): FrameMsg(std::move(k)) {
}

SendFrameMsg::SendFrameMsg(const H9frame& frame): FrameMsg(frame) {
}
