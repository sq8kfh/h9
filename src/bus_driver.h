/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-04-28.
 *
 * Copyright (C) 2019-2023 Kamil Palkowski. All rights reserved.
 */

#pragma once

#include "config.h"

#include <spdlog/spdlog.h>
#include <string>

#include "busframe.h"
#include "h9frame.h"
#include "metrics_collector.h"

class BusDriver {
  private:
    MetricsCollector::counter_t& sent_frames_counter;
    MetricsCollector::counter_t& received_frames_counter;

  protected:
    int socket_fd;

    std::shared_ptr<spdlog::logger> logger;

    virtual int recv_data(H9frame* frame) = 0;
    virtual int send_data(std::shared_ptr<BusFrame> busframe) = 0;

    void frame_sent_correctly(std::shared_ptr<BusFrame> busframe);
    void frame_sent_incorrectly(std::shared_ptr<BusFrame> busframe);
  public:
    const std::string driver_name;
    const std::string name;

    explicit BusDriver(const std::string& name, std::string driver_name);
    virtual ~BusDriver() = default;

    // TODO: uporzadowac zwracana wartosci i obsluge bledow - logowanie najlepiej wywalic na zewnatrz

    int get_scoket();
    virtual int open() = 0;
    virtual void close();

    int send_frame(std::shared_ptr<BusFrame> busframe);
    int recv_frame(BusFrame* busframe);
};
