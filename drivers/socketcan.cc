#include "socketcan.h"

SocketCAN::SocketCAN(BusMgr::EventCallback event_callback):
        Driver(std::move(event_callback)) {

}

void SocketCAN::open() {

}

void SocketCAN::recv_data() {

}

void SocketCAN::send_data(const H9frame& frame) {

}
