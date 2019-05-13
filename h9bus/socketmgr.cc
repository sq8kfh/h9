#include "socketmgr.h"

#include <system_error>
#include <string>
#include <iostream>

SocketMgr::SocketMgr() {
    FD_ZERO(&event_socket_set);
}

void SocketMgr::register_socket(Socket *socket) {
    //TODO: assert(socket_map.count(socket->getSocket()) == 0);
    if (socket->get_socket() != 0) {
        FD_SET(socket->get_socket(), &event_socket_set);
        socket_map[socket->get_socket()] = socket;
    }
    socket->setSocketMgr(this);
}

void SocketMgr::unregister_socket(Socket *socket) {
    //TODO: assert(socket_map.count(socket->getSocket()) > 0);
    if (socket->get_socket() != 0) {
        FD_CLR(socket->get_socket(), &event_socket_set);
        socket_map.erase(socket->get_socket());
    }
    socket->setSocketMgr(nullptr);
}

void SocketMgr::select_loop() {
    while (!socket_map.empty()) {
        fd_set rfds;
        FD_COPY(&event_socket_set, &rfds);

        int retval = select(socket_map.rbegin()->first+1, &rfds, nullptr, nullptr, nullptr);
        std::cout << "select\n";
        if (retval == -1) {
            throw std::system_error(errno, std::generic_category(), __FILE__ + std::string(":") + std::to_string(__LINE__));
        }
        else if (retval) {
            for (auto &it : socket_map) {
                if (FD_ISSET(it.first, &rfds))
                    it.second->on_select();
            }
        }
    }
}
