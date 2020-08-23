/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-29.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "expression.h"
#include "bus/h9frame.h"
#include "protocol/framemsg.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"

const char* NodeExp::completion_list[] = {
        "reg",
        "restart",
        nullptr
};

const char* NodeRegExp::completion_list[] = {
        "get",
        "setbit",
        "clearbit",
        "togglebit",
        "set1",
        "set2",
        "set3",
        "set4",
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
        if (raw_msg.get_type() == GenericMsg::Type::FRAME) {
            FrameMsg msg = std::move(raw_msg);
            H9frame frame = msg.get_frame();

            if (frame.source_id == reg->node->node_id
                && frame.type == H9frame::Type::REG_VALUE
                && frame.dlc > 1
                && frame.data[0] == reg->reg_number) {

                std::uint64_t tmp = static_cast<int>(frame.data[1]);
                for(int i = 2; i < frame.dlc; ++i) {
                    tmp <<= 8;
                    tmp |= frame.data[i];
                }
                std::cout << tmp << std::endl;
                break;
            }
            else if (frame.source_id == reg->node->node_id
                     && frame.type == H9frame::Type::ERROR
                     && frame.dlc == 1) {

                int err_num = static_cast<int>(frame.data[0]);
                std::cout << "Whoops, looks like something went wrong: ";
                std::cout << err_num << " - " << H9frame::error_to_string(H9frame::from_underlying<H9frame::Error>(err_num)) << std::endl;
                break;
            }
        }
    }
    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeSetReg::print_response(CommandCtx* ctx) {
    while (true) {
        GenericMsg raw_msg = ctx->get_connector()->recv();
        if (raw_msg.get_type() == GenericMsg::Type::FRAME) {
            FrameMsg msg = std::move(raw_msg);
            H9frame frame = msg.get_frame();

            if (frame.source_id == reg->node->node_id
                && frame.type == H9frame::Type::REG_EXTERNALLY_CHANGED
                && frame.dlc > 1
                && frame.data[0] == reg->reg_number) {

                std::uint64_t tmp = static_cast<int>(frame.data[1]);
                for(int i = 2; i < frame.dlc; ++i) {
                    tmp <<= 8;
                    tmp |= frame.data[i];
                }
                std::cout << tmp << std::endl;
                break;
            }
            else if (frame.source_id == reg->node->node_id
                     && frame.type == H9frame::Type::ERROR
                     && frame.dlc == 1) {

                int err_num = static_cast<int>(frame.data[0]);
                std::cout << "Whoops, looks like something went wrong: ";
                std::cout << err_num << " - " << H9frame::error_to_string(H9frame::from_underlying<H9frame::Error>(err_num)) << std::endl;
                break;
            }
        }
    }
}

void NodeSetBit::operator()(CommandCtx* ctx) {
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::SET_BIT;
    frame.dlc = 2;
    frame.data[0] = reg->reg_number;
    frame.data[1] = bit_num;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeClearBit::operator()(CommandCtx* ctx) {
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::CLEAR_BIT;
    frame.dlc = 2;
    frame.data[0] = reg->reg_number;
    frame.data[1] = bit_num;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeToggleBit::operator()(CommandCtx* ctx) {
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::TOGGLE_BIT;
    frame.dlc = 2;
    frame.data[0] = reg->reg_number;
    frame.data[1] = bit_num;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeSet1Reg::operator()(CommandCtx* ctx) {
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

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeSet2Reg::operator()(CommandCtx* ctx) {
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::SET_REG;
    frame.dlc = 3;
    frame.data[0] = reg->reg_number;
    frame.data[1] = value << 8;
    frame.data[2] = value;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeSet3Reg::operator()(CommandCtx* ctx) {
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::SET_REG;
    frame.dlc = 4;
    frame.data[0] = reg->reg_number;
    frame.data[1] = value << 16;
    frame.data[2] = value << 8;
    frame.data[3] = value;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}

void NodeSet4Reg::operator()(CommandCtx* ctx) {
    H9frame frame;

    frame.priority = H9frame::Priority::LOW;
    frame.source_id = ctx->get_source_id();
    frame.destination_id = reg->node->node_id;
    frame.type = H9frame::Type::SET_REG;
    frame.dlc = 5;
    frame.data[0] = reg->reg_number;
    frame.data[1] = value << 24;
    frame.data[2] = value << 16;
    frame.data[3] = value << 8;
    frame.data[4] = value;

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::FRAME));
    ctx->get_connector()->send(SendFrameMsg(frame));

    print_response(ctx);

    ctx->get_connector()->send(SubscribeMsg(SubscribeMsg::Content::NONE));
}
