#ifndef _H9_SENDFRAMEMSG_H_
#define _H9_SENDFRAMEMSG_H_

#include "framemsg.h"
#include "bus/h9frame.h"


class SendFrameMsg: public FrameMsg<GenericMsg::Type::SEND_FRAME> {
public:
    SendFrameMsg(GenericMsg&& k);
    SendFrameMsg(const H9frame& frame);
};


#endif //_H9_SENDFRAMEMSG_H_
