#include "driver.h"

void Driver::on_frame_recv(const H9frame& frame) {
    printf("recv %s\n", _bus_id.c_str());
}

void Driver::on_frame_send(const H9frame& frame) {
    printf("send %s\n", _bus_id.c_str());
    send_queue.pop();
    if (send_queue.size() > 0) {
        send_data(send_queue.front());
    }
}

Driver::Driver(const std::string &bus_id): _bus_id(bus_id) {
}

void Driver::close() {
    int socket = get_socket();
    set_socket(0);
    ::close(socket);
}

void Driver::send_frame(const H9frame& frame) {
    bool queue_empty = send_queue.empty();
    send_queue.push(frame);

    if (queue_empty) {
        send_data(send_queue.front());
    }
}

void Driver::on_select() {
    recv_data();
}

Driver::~Driver() {
    if (get_socket() > 0)
        close();
}
