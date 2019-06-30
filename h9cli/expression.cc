/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-29.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "expression.h"
#include "bus/h9frame.h"
#include "protocol/framereceivedmsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"

const char* NodeExp::completion_list[] = {
        "reg",
        "restart",
        nullptr
};

const char* NodeRegExp::completion_list[] = {
        "get",
        "set",
        nullptr
};

void NodeRestart::operator()(CommandCtx* ctx) {
    std::cout << "restart\n";
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = node->node_id;
    frame.type = H9frame::Type::NODE_RESET;
    frame.dlc = 0;

    ctx->get_connector()->send(SendFrameMsg(frame));
}

void NodeGetReg::operator()(CommandCtx* ctx) {
    std::cout << "get reg\n";
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::GET_REG;
    frame.dlc = 1;
    frame.data[0] = reg->reg_number;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));
    while (true) {
        GenericMsg raw_msg = ctx->get_connector()->recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME_RECEIVED) {
            FrameReceivedMsg msg = std::move(raw_msg);
            H9frame frame = msg.get_frame();

            if (frame.source_id == reg->node->node_id
                && frame.type == H9frame::Type::REG_VALUE
                && frame.dlc > 1
                && frame.data[0] == reg->reg_number) {

                std::cout << static_cast<int>(frame.data[1]) << std::endl;
                break;
            }
        }
    }
    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeSetReg::operator()(CommandCtx* ctx) {
    std::cout << "set reg\n";
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::SET_REG;
    frame.dlc = 2;
    frame.data[0] = reg->reg_number;
    frame.data[1] = value;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));
    while (true) {
        GenericMsg raw_msg = ctx->get_connector()->recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME_RECEIVED) {
            FrameReceivedMsg msg = std::move(raw_msg);
            H9frame frame = msg.get_frame();

            if (frame.source_id == reg->node->node_id
                && frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED
                && frame.dlc > 1
                && frame.data[0] == reg->reg_number) {

                std::cout << static_cast<int>(frame.data[1]) << std::endl;
                break;
            }
        }
    }
    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}
