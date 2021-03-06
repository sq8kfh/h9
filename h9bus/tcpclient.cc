#include <utility>

/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-15.
 *
 * Copyright (C) 2019-2020 Kamil Palkowski. All rights reserved.
 */

#include "tcpclient.h"

#include <system_error>
#include <utility>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "common/logger.h"
#include "protocol/errormsg.h"
#include "protocol/genericmsg.h"


void TcpClient::recv_msg() {
    GenericMsg msg;
    int res = h9socket.recv(msg);
    if (res <=0) {
        if (res == 0 || errno == ECONNRESET) {  /*Connection reset by peer*/
            close();
            return;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK) { //incomplete message
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }

    //TODO: proces parser error
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
    if (msg.get_type() == GenericMsg::Type::IDENTIFICATION) {
        IdentificationMsg ident_msg = std::move(msg);
        entity = ident_msg.get_entity();
    }
    else {
        recv_msg_callback(this, msg);
    }
}

TcpClient::TcpClient(TNewMsgCallback new_msg_callback, int sockfd):
        recv_msg_callback(std::move(new_msg_callback)),
        h9socket(sockfd),
        active_subscription(0) {

    set_socket(sockfd, true);
    //h9socket.connect();
}

bool TcpClient::is_subscriber() {
    return active_subscription;
}

void TcpClient::subscriber(int active) {
    active_subscription = active;
}

void TcpClient::send(GenericMsg& msg) {
    int res = h9socket.send(msg);

    if (res <= 0) {
        if (res == 0 || errno == ECONNRESET || errno == EPIPE || errno == EBADF /*|| errno == EPROTOTYPE*/) {
            // EPROTOTYPE - Protocol wrong type for socket - on mac os during high trafic - i don't know why

            close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

TcpClient::~TcpClient() {
}

void TcpClient::on_close() noexcept {
    h9socket.close();
    disconnected();
}

std::string TcpClient::get_remote_address() noexcept {
    return std::move(h9socket.get_remote_address());
}

std::string TcpClient::get_remote_port() noexcept {
    return std::move(h9socket.get_remote_port());
}

std::string TcpClient::get_entity() noexcept {
    return entity;
}

std::string TcpClient::get_client_idstring() noexcept {
    return get_entity() + "@" + get_remote_address() + ":" + get_remote_port();
}

void TcpClient::on_select() {
    recv_msg();
}
