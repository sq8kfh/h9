#include "dummy.h"

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>

Dummy::Dummy(const std::string &bus_id): Driver(bus_id) {
}

void Dummy::open() {
    int fd = ::open("/dev/null", O_WRONLY);
    if (fd == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    set_socket(fd);
}

void Dummy::recv_data() {

}

void Dummy::send_data(const H9frame& frame) {
    if( write(get_socket(), &frame, sizeof(frame)) == -1) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    else {
        on_frame_send(frame);
    }
}
