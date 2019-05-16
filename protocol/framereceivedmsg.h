#ifndef _H9_FRAMERECEIVEDMSG_H_
#define _H9_FRAMERECEIVEDMSG_H_

#include "concretizemsg.h"

class FrameReceivedMsg: public ConcretizeMsg<GenericMsg::Type::FRAME_RECEIVED> {
public:
    FrameReceivedMsg();
};


#endif //_H9_FRAMERECEIVEDMSG_H_
