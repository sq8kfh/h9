/*
 * H9 project
 *
 * Created by crowx on 2023-09-06.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "bus.h"

#include <future>
#include <utility>

#include "h9d_configurator.h"

void Bus::recv_thread() {
    SPDLOG_LOGGER_INFO(logger, "The bus manager is running...");

    for (const auto& [socket, driver] : bus) {
        event_notificator.attach_socket(socket);
    }
    while (run) {
        int number_of_events = event_notificator.wait();
        if (event_notificator.is_async_event(number_of_events)) {
            while (true) {
                SPDLOG_LOGGER_TRACE(logger, "Event queue loop");
                std::shared_ptr<BusFrame> bus_frame = nullptr;

                send_queue_mtx.lock();
                bool queue_empty = send_queue.empty();
                if (!queue_empty) {
                    bus_frame = send_queue.top();
                    send_queue.pop();
                    //size_of_send_queue = send_queue.size();
                }
                send_queue_mtx.unlock();

                if (queue_empty)
                    break;

                bus_frame->set_number_of_active_bus(bus.size());
                if (!bus_frame->raw()) {
                    bus_frame->source_id(_bus_id);
                    bus_frame->seqnum(next_seqnum);
                    ++next_seqnum;
                }

                ++sent_frames_counter;
                ++(*sent_frames_counter_by_type[H9frame::to_underlying(bus_frame->type())]);

                for (const auto& [socket, bus_driver] : bus) {
                    bus_driver->send_frame(bus_frame);

                    SPDLOG_LOGGER_DEBUG(frames_logger, "Send frame {} to {}.", *bus_frame, bus_driver->name);
                    frames_sent_file_logger->info(SimpleJSONBusFrameWraper(*bus_frame));
                }
            }
        }
        else {
            for (const auto& [socket, bus_driver] : bus) {
                if (event_notificator.is_socket_event(number_of_events, socket)) {

                    BusFrame frame;
                    bus_driver->recv_frame(&frame);
                    ++received_frames_counter;
                    ++(*received_frames_counter_by_type[H9frame::to_underlying(frame.type())]);

                    SPDLOG_LOGGER_DEBUG(frames_logger, "Recv frame {} from {}.", frame, bus_driver->name);
                    frames_recv_file_logger->info(SimpleJSONBusFrameWraper(frame));

                    notify_frame_observer(frame);
                }
            }
        }
    }
}

Bus::Bus():
    run(true),
    sent_frames_counter(MetricsCollector::make_counter("bus.send_frames")),
    received_frames_counter(MetricsCollector::make_counter("bus.received_frames")) {
    //size_of_send_queue(MetricsCollector::make_counter("bus.size_of_send_queue")) {
    logger = spdlog::get(H9dConfigurator::bus_logger_name);
    frames_logger = spdlog::get(H9dConfigurator::frames_logger_name);
    frames_recv_file_logger = spdlog::get(H9dConfigurator::frames_recv_to_file_logger_name);
    frames_sent_file_logger = spdlog::get(H9dConfigurator::frames_recv_to_file_logger_name);

    next_seqnum = 0;

    for (int i =0; i < number_of_frame_types; ++i) {
        sent_frames_counter_by_type[i] = MetricsCollector::make_counter_ptr("bus.frames[type=" + std::string(H9frame::type_to_string(H9frame::from_underlying<H9frame::Type>(i))) + "].send");
        received_frames_counter_by_type[i]= MetricsCollector::make_counter_ptr("bus.frames[type=" + std::string(H9frame::type_to_string(H9frame::from_underlying<H9frame::Type>(i))) + "].received");
    }

    SPDLOG_LOGGER_INFO(logger, "Created buses manager with '{}' I/O event notification mechanism.", IOEventQueue::notification_mechanism_name);
}

Bus::~Bus() {
    run = false;

    if (recv_thread_desc.joinable())
        recv_thread_desc.join();

    for (const auto& [socket, driver] : bus) {
        delete driver;
    }
}

void Bus::bus_id(std::uint16_t bus_id) {
    _bus_id = bus_id;
}

void Bus::add_driver(BusDriver* bus_driver) {
    SPDLOG_LOGGER_INFO(logger, "Adding a bus interface {} (driver: {})...", bus_driver->name, bus_driver->driver_name);
    bus[bus_driver->open()] = bus_driver;
}

void Bus::activate() {
    recv_thread_desc = std::thread([this]() {
        this->recv_thread();
    });
}

int Bus::send_frame(ExtH9Frame frame, bool raw) {
    std::future<SendFrameResult> send_future = send_frame_noblock(std::move(frame), raw);

    return send_future.get().seqnum;
}

std::future<SendFrameResult> Bus::send_frame_noblock(ExtH9Frame frame, bool raw) {
    std::shared_ptr<BusFrame> busframe(new BusFrame(std::move(frame), raw));

    send_queue_mtx.lock();
    bool queue_empty = send_queue.empty();
    send_queue.push(std::shared_ptr<BusFrame>(busframe));
    send_queue_mtx.unlock();

    if (queue_empty) {
        event_notificator.trigger_async_event();
    }

    return std::move(busframe->get_send_promise().get_future());
}
