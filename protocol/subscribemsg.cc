/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-19.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "subscribemsg.h"

SubscribeMsg::SubscribeMsg(GenericMsg&& k): ConcretizeMsg(std::move(k)) {

}

SubscribeMsg::SubscribeMsg(SubscribeMsg::Content content): ConcretizeMsg() {

}

SubscribeMsg::Content SubscribeMsg::get_content() {
    return SubscribeMsg::Content::FRAME;
}
