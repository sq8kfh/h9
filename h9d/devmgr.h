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
#include <atomic>
#include <thread>
#include <map>
#include <queue>
#include "bus.h"
#include "bus/h9frame.h"
#include "dctx.h"
#include "device.h"
#include "frameobserver.h"


class TCPClientThread;

class DevMgr: public FrameObserver {
private:
    Bus* const h9bus;
    void on_frame_recv(H9frame frame) override;

    std::map<std::uint16_t, Device*> devices_map;

    TCPClientThread *_client; //POC

    std::mutex frame_queue_mtx;
    std::condition_variable frame_queue_cv; //TODO: counting_semaphore (C++20)?
    std::queue<H9frame> frame_queue;

    std::atomic_bool devices_update_thread_run;
    std::thread devices_update_thread_desc;
    void devices_update_thread();

    void add_device(std::uint16_t node_id, std::uint16_t node_type, std::uint16_t node_version);
public:
    explicit DevMgr(Bus *bus);
    DevMgr(const DevMgr &a) = delete;
    ~DevMgr();
    void load_config(DCtx *ctx);
    void discover();

    void add_dev_observer(TCPClientThread *client);
};


#endif //H9_DEVMGR_H
