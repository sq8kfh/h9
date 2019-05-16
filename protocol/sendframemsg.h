#ifndef _H9_SENDFRAMEMSG_H_
#define _H9_SENDFRAMEMSG_H_


#include "concretizemsg.h"
#include "bus/h9frame.h"

class SendFrameMsg: public ConcretizeMsg<GenericMsg::Type::SEND_FRAME> {
public:
    SendFrameMsg(GenericMsg&& k);
    SendFrameMsg(const H9frame& frame);
    H9frame get_frame();
};


#endif //_H9_SENDFRAMEMSG_H_
