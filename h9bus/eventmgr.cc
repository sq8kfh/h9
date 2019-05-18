#include "eventmgr.h"

#include "tcpclient.h"
#include "busmgr.h"
#include "protocol/sendframemsg.h"
#include "protocol/subscribemsg.h"
#include "protocol/framereceivedmsg.h"
#include "common/logger.h"

EventMgr::EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr):
        _ctx(ctx),
        _bus_mgr(bus_mgr),
        _server_mgr(server_mgr) {
}

void EventMgr::flush_frame_queue() {
    auto& frame_queue = _bus_mgr->get_recv_queue();
    while (!frame_queue.empty()) {
        flush_frame(frame_queue.front().first, frame_queue.front().second);
        frame_queue.pop();
    }
}

void EventMgr::flush_msg_queue() {
    auto& msg_queue = _server_mgr->get_recv_queue();
    while (!msg_queue.empty()) {
        flush_msg(msg_queue.front().first, msg_queue.front().second);
        msg_queue.pop();
    }
}

void EventMgr::flush_all() {
    flush_frame_queue();
    flush_msg_queue();
}

void EventMgr::flush_frame(const std::string& bus_id, const H9frame& frame) {
    h9_log_debug("EventMgr::flush_frame(%s, ?)", bus_id.c_str());
    FrameReceivedMsg msg(frame);
    _server_mgr->send_msg_to_subscriber(msg);
}

void EventMgr::flush_msg(int client_socket, GenericMsg& msg) {
    h9_log_debug("EventMgr::flush_msg(%d, ?)", client_socket);
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            _bus_mgr->send_frame(tmp);
            break;
        }
        case GenericMsg::Type::SUBSCRIBE: {
            SubscribeMsg sc_msg = std::move(msg);
            _server_mgr->get_cient(client_socket)->subscriber(1);
            break;
        }
        default:
            //TODO: send error msg - invalid msg
            h9_log_err("recv unknown msg: %d", msg.get_type());
            break;
    }
}
