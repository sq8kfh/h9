#include "tcpclient.h"

#include <system_error>
#include <iostream>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>

void TcpClient::recv() {
    if (data_to_read > 0) {
        recv_data();
    }
    else {
        recv_header();
    }
}

void TcpClient::recv_header() {
    static size_t recv_bytes = 0;
    std::uint32_t recv_header = 0;
    ssize_t nbyte = ::recv(get_socket(), reinterpret_cast<char*>(&recv_header) + recv_bytes, sizeof(recv_header) - recv_bytes, 0);
    if (nbyte <= 0) {
        if (nbyte == 0) {
            on_close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    recv_bytes += nbyte;
    if (recv_bytes == sizeof(recv_header)) {
        recv_bytes = 0;
        data_to_read = ntohl(recv_header);
    }
}

void TcpClient::recv_data() {
    static size_t buf_len = 4096;
    static char* buf = new char[buf_len];

    size_t bytes_to_recv = data_to_read - recv_data_buf.size();

    ssize_t nbyte = ::recv(get_socket(), buf, buf_len < bytes_to_recv ? buf_len : bytes_to_recv, 0);
    if (nbyte <= 0) {
        if (nbyte == 0) {
            on_close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    recv_data_buf.append(buf, nbyte);

    if (recv_data_buf.size() == data_to_read) {

        std::cout << recv_data_buf << std::endl;

        data_to_read = 0;
        recv_data_buf.clear();
    }
}

TcpClient::TcpClient(ServerMgr::EventCallback event_callback, int sockfd, std::string remote_address, std::uint16_t remote_port):
        _event_callback(event_callback),
        remote_address(std::move(remote_address)),
        remote_port(remote_port) {
    data_to_read = 0;
    set_socket(sockfd);
}

TcpClient::~TcpClient() {
    if (get_socket() > 0) {
        ::close(get_socket());
        set_socket(0);
    }
}

void TcpClient::close() {
    int socket = get_socket();
    _event_callback.on_client_close(get_socket());
    set_socket(0);
    ::close(socket);
}

void TcpClient::on_select() {
    recv();
}
