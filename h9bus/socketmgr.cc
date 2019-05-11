#include "socketmgr.h"

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
        if (retval == -1) {
            //TODO: error message
            break;
        }
        else if (retval) {
            for (auto &it : socket_map) {
                it.second->on_select();
            }
        }
    }
}
