/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_DEVMGR_H
#define H9_DEVMGR_H

#include "config.h"
#include "busobserver.h"
#include "dctx.h"


class TCPClientThread;

class DevMgr: public BusObserver {
private:
    void on_frame_recv(H9frame frame) override;

    TCPClientThread *_client;
public:
    explicit DevMgr(Bus *bus);
    DevMgr(const DevMgr &a) = delete;
    ~DevMgr();
    void load_config(DCtx *ctx);
    void discover();

    void add_dev_observer(TCPClientThread *client);
};


#endif //H9_DEVMGR_H
