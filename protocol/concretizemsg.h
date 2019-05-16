#ifndef H9_CONCRETIZEMSG_H
#define H9_CONCRETIZEMSG_H

#include "genericmsg.h"

template<GenericMsg::Type msg_type>
class ConcretizeMsg: public GenericMsg {
protected:
    explicit ConcretizeMsg(GenericMsg&& k) noexcept: GenericMsg(std::move(k)) {
    }
    ConcretizeMsg(): GenericMsg(msg_type) {
    }
public:
    virtual GenericMsg::Type get_type() {
        return msg_type;
    }
};

#endif //H9_CONCRETIZEMSG_H
