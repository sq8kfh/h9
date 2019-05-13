#include "dummy.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>

Dummy::Dummy(BusMgr::RecvFrameCallback recv_frame_callback, BusMgr::SendFrameCallback send_frame_callback):
        Driver(recv_frame_callback, send_frame_callback) {
}

void Dummy::open() {
    write_only_socket = ::open("/dev/null", O_WRONLY);
    if (write_only_socket == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    //set_socket(fd); //write only driver - don't add to select set
}

void Dummy::recv_data() {

}

void Dummy::send_data(const H9frame& frame) {
    if( write(write_only_socket, &frame, sizeof(frame)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        on_frame_send(frame);
    }
}
