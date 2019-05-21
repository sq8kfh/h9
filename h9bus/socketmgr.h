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
#include <map>
#include <exception>
#include <functional>
#include <unistd.h>


class SocketMgr {
public:
    class Socket;
private:
    std::map<int, Socket*> socket_map;
    fd_set event_socket_set;
public:
    SocketMgr();
    void register_socket(Socket *socket);
    void unregister_socket(Socket *socket);
    void select_loop(std::function<void(void)> after_select_callback = nullptr, std::function<void(void)> cron_func = nullptr);

    class Socket {
    private:
        int _socket;
    protected:
        void set_socket(int socket) {
            _socket = socket;
        }
    public:
        class CloseSocketException: public std::exception {
        public:
            Socket* _socket;
            explicit CloseSocketException(Socket *socket): _socket(socket) {}
        };

        Socket(): _socket(0) {
        }
        virtual ~Socket() {
            set_socket(0);
        }
        int get_socket() {
            return _socket;
        }
        void on_close() {
            throw CloseSocketException(this);
        }
        virtual void on_select() = 0;
        virtual void close() = 0;
    };
};


#endif //_H9_SOCKETMGR_H_
