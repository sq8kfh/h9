/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-15.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#include "tcpclient.h"

#include <system_error>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common/logger.h"
#include "protocol/errormsg.h"
#include "protocol/genericmsg.h"

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
        if (nbyte == 0 || errno == ECONNRESET) {  /*Connection reset by peer*/
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
        if (nbyte == 0 || errno == ECONNRESET) {
            on_close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    recv_data_buf.append(buf, nbyte);

    if (recv_data_buf.size() == data_to_read) {
        std::string tmp = std::move(recv_data_buf);

        data_to_read = 0;
        recv_data_buf.clear();

        recv_msg(tmp);
    }
}

void TcpClient::recv_msg(const std::string& msg_str) {
    //TODO: proces parser error
    GenericMsg msg = GenericMsg(msg_str);
    std::string error_msg;
    if (msg.validate_msg(&error_msg)) {
        h9_log_warn("tcp client (socket: %d) recv invalid msg: [%s]", get_socket(), error_msg.c_str());
        ErrorMsg err_msg = {ErrorMsg::ErrorNumber::INVALID_MESSAGE_SCHEMA, error_msg};
        try {
            send(err_msg);
        }
        catch (SocketMgr::Socket::CloseSocketException& e) {
            h9_log_info("request to terminate the connection during sending a error message (socket: %d)", get_socket());
            throw;
        }
        return;
    }
    _event_callback.on_msg_recv(get_socket(), msg);
}

TcpClient::TcpClient(ServerMgr::EventCallback event_callback, int sockfd, std::string remote_address, std::uint16_t remote_port):
        _event_callback(event_callback),
        remote_address(std::move(remote_address)),
        remote_port(remote_port) {

    data_to_read = 0;
    active_subscription = 0;

    set_socket(sockfd);

    int set = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int)) < 0) {
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

bool TcpClient::is_subscriber() {
    return active_subscription;
}

void TcpClient::subscriber(int active) {
    active_subscription = active;
}

void TcpClient::send(GenericMsg& msg) {
    std::string raw_msg = msg.serialize();
    std::uint32_t header = htonl(raw_msg.size());

    ssize_t nbyte = ::send(get_socket(), &header, sizeof(header), 0);
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ECONNRESET || errno == EPIPE) {
            on_close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    nbyte = ::send(get_socket(), raw_msg.c_str(), raw_msg.size(), 0);
    if (nbyte <= 0) {
        if (nbyte == 0 || errno == ECONNRESET || errno == EPIPE) {
            on_close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

TcpClient::~TcpClient() {
    //h9_log_debug("TcpClient::~TcpClient (socket: %d)", get_socket());
    if (get_socket() > 0) {
        ::close(get_socket());
        set_socket(0);
    }
}

void TcpClient::close() {
    int socket = get_socket();
    ::close(socket);
    _event_callback.on_client_close(socket);
}

void TcpClient::on_select() {
    recv();
}
