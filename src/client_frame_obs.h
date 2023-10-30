/*
 * H9 project
 *
 * Created by crowx on 2023-09-12.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "frameobserver.h"

class TCPClientThread;

class ClientFrameObs: public FrameObserver {
  private:
    TCPClientThread* client;
    void on_frame_recv(const ExtH9Frame& frame) override;
    void on_frame_send(const ExtH9Frame& frame) override;
  public:
    ClientFrameObs(TCPClientThread* tcp_client_thread, FrameSubject* subject, H9FrameComparator comparator);
};
