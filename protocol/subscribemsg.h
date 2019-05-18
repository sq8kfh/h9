#ifndef _H9_SUBSCRIBEMSG_H_
#define _H9_SUBSCRIBEMSG_H_

#include "common/logger.h"
#include "concretizemsg.h"


class SubscribeMsg: public ConcretizeMsg<GenericMsg::Type::SUBSCRIBE> {
public:
    enum class Content {
        NONE = 0,
        FRAME,
    };
    SubscribeMsg(GenericMsg&& k);
    SubscribeMsg(Content content);
    Content get_content();
};


#endif //_H9_SUBSCRIBEMSG_H_
