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
