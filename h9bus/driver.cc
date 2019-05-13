#include "driver.h"

void Driver::on_frame_recv(const H9frame& frame) {
    _recv_frame_callback(frame);
}

void Driver::on_frame_send(const H9frame& frame) {
    printf("send\n");
    send_queue.pop();
    if (send_queue.size() > 0) {
        send_data(send_queue.front());
    }
}

Driver::Driver(BusMgr::RecvFrameCallback recv_frame_callback):
        _recv_frame_callback(recv_frame_callback) {
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
