#include "socketcan.h"

SocketCAN::SocketCAN(BusMgr::RecvFrameCallback recv_frame_callback): Driver(recv_frame_callback) {

}

void SocketCAN::open() {

}

void SocketCAN::recv_data() {

}

void SocketCAN::send_data(const H9frame& frame) {

}
