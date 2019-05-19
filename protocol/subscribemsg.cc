#include "subscribemsg.h"

SubscribeMsg::SubscribeMsg(GenericMsg&& k): ConcretizeMsg(std::move(k)) {

}

SubscribeMsg::SubscribeMsg(SubscribeMsg::Content content): ConcretizeMsg() {

}

SubscribeMsg::Content SubscribeMsg::get_content() {
    return SubscribeMsg::Content::FRAME;
}
