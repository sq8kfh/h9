/*
 * H9 project
 *
 * Created by crowx on 2023-09-06.
 *
 * Copyright (C) 2023 Kamil Palkowski. All rights reserved.
 */

#include "bus.h"
#include <iostream>
#include <future>


template<class IOEventQueue>
void Bus<IOEventQueue>::recv_thread() {
    for (const auto& [socket, value] : bus) {
        event_notificator.attach_socket(socket);
    }
    while (run) {
        int number_of_events = event_notificator.wait();
        if (event_notificator.is_async_event(number_of_events)) {

            while (true) {
                std::cout << "run" << std::endl;
                BusFrame *bus_frame = nullptr;

                send_queue_mtx.lock();
                bool queue_empty = send_queue.empty();
                if (!queue_empty) {
                    bus_frame = send_queue.top();
                    send_queue.pop();
                }
                send_queue_mtx.unlock();

                if (queue_empty) break;

                std::cout << "run not empty" << std::endl;
                bus_frame->set_number_of_active_bus(bus.size());
                for (const auto &[socket, bus_driver]: bus) {
                    bus_driver->send_frame(bus_frame);
                }
            }

            send_orphans_mtx.lock();
            for (auto it = send_orphans.begin(); it != send_orphans.end();) {
                if ((*it)->is_sent_finish()) {
                    delete *it;
                    it = send_orphans.erase(it);
                }
                else
                    ++it;
            }
            send_orphans_mtx.unlock();
        }
        else {
            for (const auto &[socket, bus_driver]: bus) {
                if (event_notificator.is_socket_event(number_of_events, socket)) {

                    BusFrame frame;
                    bus_driver->recv_frame(&frame);
                    notify_frame_observer(frame);
                }
            }
        }
    }
}

template<class IOEventQueue>
Bus<IOEventQueue>::Bus(): run(true) {

}

template<class IOEventQueue>
Bus<IOEventQueue>::~Bus() {
    run = false;

    if (recv_thread_desc.joinable())
        recv_thread_desc.join();
}

template<class IOEventQueue>
void Bus<IOEventQueue>::add_driver(BusDriver *bus_driver) {
    bus[bus_driver->open()] = bus_driver;
}

template<class IOEventQueue>
void Bus<IOEventQueue>::activate() {
    recv_thread_desc = std::thread([this]() {
        this->recv_thread();
    });
}

template<class IOEventQueue>
int Bus<IOEventQueue>::send_frame(ExtH9Frame frame) {
    BusFrame busframe = std::move(frame);

    std::future<int> send_future = busframe.get_send_promise().get_future();

    send_queue_mtx.lock();
    bool queue_empty = send_queue.empty();
    send_queue.push(&busframe);
    send_queue_mtx.unlock();

    if (queue_empty) {
        event_notificator.trigger_async_event();
    }

    return send_future.get();
}

template<class IOEventQueue>
int Bus<IOEventQueue>::send_frame_noblock(ExtH9Frame frame) {
    BusFrame *busframe = new BusFrame(std::move(frame));

    send_queue_mtx.lock();
    bool queue_empty = send_queue.empty();
    send_queue.push(busframe);
    send_queue_mtx.unlock();

    if (queue_empty) {
        event_notificator.trigger_async_event();
    }

    send_orphans_mtx.lock();
    send_orphans.push_back(busframe);
    send_orphans_mtx.unlock();

    return 0;
}
