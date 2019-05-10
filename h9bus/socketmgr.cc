#include "socketmgr.h"

SocketMgr::SocketMgr() {
    FD_ZERO(&event_socket_set);
}

void SocketMgr::register_socket(Socket *socket) {
    //TODO: assert(socket_map.count(socket->getSocket()) == 0);
    if (socket->getSocket() != 0) {
        FD_SET(socket->getSocket(), &event_socket_set);
        socket_map[socket->getSocket()] = socket;
    }
    socket->setSocketMgr(this);
}

void SocketMgr::unregister_socket(Socket *socket) {
    //TODO: assert(socket_map.count(socket->getSocket()) > 0);
    if (socket->getSocket() != 0) {
        FD_CLR(socket->getSocket(), &event_socket_set);
        socket_map.erase(socket->getSocket());
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
                it.second->onSelect();
            }
        }
    }
}
