/*
 * Created by crowx on 29/10/2023.
 *
 */

#include "dev_status_observer.h"

#include "node_dev_mgr.h"
#include "tcpclientthread.h"

DevStatusObserver::DevStatusObserver(TCPClientThread* tcp_client_thread, NodeDevMgr* mgr):
    client(tcp_client_thread),
    mgr(mgr) {
    mgr->attach_dev_state_observer("*", this);
}

DevStatusObserver::~DevStatusObserver() {
    mgr->detach_dev_state_observer("*", this);
}

void DevStatusObserver::on_dev_state_update(const nlohmann::json& dev_status) {
    jsonrpcpp::Notification n("dev_status_update", nlohmann::json({{"status", dev_status}}));
    client->send_msg(std::make_shared<jsonrpcpp::Notification>(std::move(n)));
}
