/*
 * H9 project
 *
 * Created by crowx on 2023-09-12.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "client_frame_obs.h"

#include <jsonrpcpp/jsonrpcpp.hpp>

void ClientFrameObs::on_frame_recv(const ExtH9Frame& frame) {
    jsonrpcpp::Notification n("on_frame", nlohmann::json({{"frame", frame}}));
    client->send_msg(std::make_shared<jsonrpcpp::Notification>(std::move(n)));
}

ClientFrameObs::ClientFrameObs(TCPClientThread* tcp_client_thread, FrameSubject* subject, H9FrameComparator comparator):
    FrameObserver(subject, comparator),
    client(tcp_client_thread) {
}
