/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-05-10.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef _H9_SOCKETMGR_H_
#define _H9_SOCKETMGR_H_

#include "config.h"
#include <set>
#include <exception>
#include <functional>
#include <unistd.h>


class SocketMgr {
public:
    class Socket;
private:
    std::set<Socket*> socket_set;
    int socket_max;
    fd_set event_socket_set;
public:
    SocketMgr() noexcept;
    void register_socket(Socket *socket) noexcept;
    void unregister_socket(Socket *socket) noexcept;
    void select_loop(std::function<void(void)> after_select_callback = nullptr, std::function<void(void)> cron_func = nullptr);

    class Socket {
    private:
        int _socket;
        bool _connected;
    protected:
        void set_socket(int socket, bool connected) noexcept {
            _socket = socket;
            _connected = connected;
        }
    public:
        class CloseSocketException: public std::exception {
        public:
            Socket* _socket;
            explicit CloseSocketException(Socket *socket): _socket(socket) {}
        };

        Socket() noexcept: _socket(0), _connected(false) {
        }
        virtual ~Socket() noexcept {
            set_socket(0, false);
        }
        int get_socket() noexcept {
            return _socket;
        }
        bool is_connected() noexcept {
            return _connected;
        }
        void disconnected() {
            _connected = false;
        }
        void close() {
            throw CloseSocketException(this);
        }
        virtual void on_select() = 0;
        virtual void on_close() noexcept = 0;
    };
};


#endif //_H9_SOCKETMGR_H_
