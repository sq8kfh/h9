#include "eventmgr.h"

#include "protocol/sendframemsg.h"
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
    h9_log_info("EventMgr::on_fame_recv");
    FrameReceivedMsg msg(frame);
    h9_log_debug("EventMgr::on_fame_recv1");
    _server_mgr->send_msg_to_subscriber(msg);
    h9_log_debug("EventMgr::on_fame_recv2");
}

void EventMgr::flush_msg(int client_socket, GenericMsg& msg) {
    switch (msg.get_type()) {
        case GenericMsg::Type::SEND_FRAME: {
            SendFrameMsg sf_msg = std::move(msg);
            H9frame tmp = sf_msg.get_frame();
            _bus_mgr->send_frame(tmp);
            break;
        }
        default:
            //TODO: send error msg - invalid msg
            h9_log_err("recv unknown msg: %d", msg.get_type());
            break;
    }
}
