/*
 * H9 project
 *
 * Created by SQ8KFH on 2020-11-19.
 *
 * Copyright (C) 2020 Kamil Palkowski. All rights reserved.
 */

#include "tcpclientthread.h"
#include <cassert>
#include <sys/errno.h>
#include "protocol/devicemethodresponsemsg.h"
#include "protocol/executemethodmsg.h"
#include "protocol/identificationmsg.h"
#include "protocol/errormsg.h"


void TCPClientThread::thread() {
    thread_running = true;
    int read_fd = h9socket.get_socket();

    event.attach_socket(read_fd);
    //try {
        while (running) {
            int n = event.wait();
            for (int i = 0; i < n; ++i) {
                if (event.is_socket_event(i, read_fd)) thread_recv_msg();
                else if (event.is_async_event(i)) thread_send_async_msg();
            }
        }
    //}
    //catch (...) {
    //    execadapter.cleanup_connection();
    //    thread_running = false;
    //    throw;
    //}
    execadapter.cleanup_connection();
    thread_running = false;
}

void TCPClientThread::thread_recv_msg() {
    GenericMsg msg;
    int res = h9socket.recv_complete_msg(msg);
    if (res <= 0) {
        if (res == 0 || errno == ECONNRESET) {  /*Connection reset by peer*/
            running = false;
            h9socket.close();
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
    if (msg.get_type() == GenericMsg::Type::IDENTIFICATION) {
        IdentificationMsg ident_msg = std::move(msg);
        entity = ident_msg.get_entity();
    }
    else if (msg.get_type() == GenericMsg::Type::EXECUTEMETHOD) {
        ExecuteMethodMsg exec_msg = std::move(msg);
        h9_log_info("Msg id %llu: execute method '%s' from client %s", exec_msg.get_id(), exec_msg.get_method_name().c_str(), get_client_idstring().c_str());

        GenericMsg ret = execadapter.execute_method(std::move(exec_msg));
        send(std::move(ret));
    }
    else if (msg.get_type() == GenericMsg::Type::EXECUTEDEVICEMETHOD) {
        DeviceMethodResponseMsg devexec_msg = std::move(msg);
        h9_log_info("Msg id %llu: execute device (%hu) method '%s' from client %s", devexec_msg.get_id(), devexec_msg.get_id(), devexec_msg.get_method_name().c_str(), get_client_idstring().c_str());

        GenericMsg ret = execadapter.execute_device_method(std::move(devexec_msg));
        send(std::move(ret));
    }
    else {
        h9_log_err("Msg id %llu: unknown type: %d from client: %s", msg.get_id(), msg.get_type(), get_client_idstring().c_str());
        ErrorMsg err_msg = {ErrorMsg::ErrorNumber::UNSUPPORTED_MESSAGE_TYPE, "Unsupported message type"};
        err_msg.set_request_id(msg.get_id());
        send(std::move(err_msg));
    }
}

void TCPClientThread::thread_send_async_msg() {
    bool queue_empty = false;
    do {
        async_msg_queue_mtx.lock();
        h9_log_debug("thread_send_async_msg", async_msg_queue.size());

        assert(!async_msg_queue.empty());

        GenericMsg msg = async_msg_queue.front();
        async_msg_queue.pop();

        if (async_msg_queue.empty()) queue_empty = true;
        async_msg_queue_mtx.unlock();

        send(std::move(msg));
    } while (!queue_empty);
}

void TCPClientThread::send(GenericMsg msg) {
    int ret = h9socket.send(msg);
    if (ret <=0) {
        if (ret == 0) {
            running = false;
            return;
        }
        throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
    }
}

TCPClientThread::TCPClientThread(int sockfd, ExecutorAdapter execadapter): h9socket(sockfd), execadapter(execadapter) {
    this->execadapter.set_client(this);

    running = true;
    client_thread_desc = std::thread([this]() {
        this->thread();
    });
}

TCPClientThread::~TCPClientThread() {
    close_connection();
    if (client_thread_desc.joinable())
        client_thread_desc.join();
    h9socket.close();
}

std::string TCPClientThread::get_remote_address() noexcept {
    return h9socket.get_remote_address();
}

std::string TCPClientThread::get_remote_port() noexcept {
    return h9socket.get_remote_port();
}

std::string TCPClientThread::get_entity() noexcept {
    return entity;
}

std::string TCPClientThread::get_client_idstring() noexcept {
    return get_entity() + "@" + get_remote_address() + ":" + get_remote_port();
}

bool TCPClientThread::is_running() {
    return thread_running;
}

void TCPClientThread::close_connection() {
    running = false;
    h9socket.shutdown_read();
}

void TCPClientThread::send_msg(GenericMsg msg) {
    async_msg_queue_mtx.lock();
    async_msg_queue.push(std::move(msg));
    if (async_msg_queue.size() == 1) {
        event.trigger_async_event();
    }
    async_msg_queue_mtx.unlock();
}
