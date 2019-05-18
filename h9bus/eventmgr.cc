#include "eventmgr.h"

#include "protocol/sendframemsg.h"
#include "protocol/framereceivedmsg.h"
#include "common/logger.h"

EventMgr::EventMgr(DaemonCtx* ctx, BusMgr* bus_mgr, ServerMgr* server_mgr):
        _ctx(ctx),
        _bus_mgr(bus_mgr),
        _server_mgr(server_mgr) {
}

void EventMgr::on_msg_recv(int client_socket, GenericMsg& msg) {
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

void EventMgr::on_fame_recv(const std::string& bus_id, const H9frame& frame) {
    h9_log_info("EventMgr::on_fame_recv");
    FrameReceivedMsg msg(frame);
    h9_log_debug("EventMgr::on_fame_recv1");
    _server_mgr->send_msg_to_subscriber(msg);
    h9_log_debug("EventMgr::on_fame_recv2");
}
