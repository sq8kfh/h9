#ifndef _H9_SOCKETMGR_H_
#define _H9_SOCKETMGR_H_

#include <map>
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
    void select_loop();


    class Socket {
    private:
        int _socket;
        SocketMgr *_socket_mgr;
        void setSocketMgr(SocketMgr *socket_mgr) {
            _socket_mgr = socket_mgr;
        }
        friend void SocketMgr::register_socket(Socket *socket);
        friend void SocketMgr::unregister_socket(Socket *socket);
    protected:
        void set_socket(int socket) {
            SocketMgr *tmp_socket_mgr = _socket_mgr;
            if (_socket != 0 && _socket_mgr != nullptr) {
                _socket_mgr->unregister_socket(this);
            }
            _socket = socket;
            if (_socket != 0 && tmp_socket_mgr != nullptr) {
                tmp_socket_mgr->register_socket(this);
            }
        }
    public:
        Socket(): _socket(0), _socket_mgr(nullptr) {
        }
        ~Socket() {
            set_socket(0);
        }
        int get_socket() {
            return _socket;
        }

        virtual void on_select() = 0;
    };
};


#endif //_H9_SOCKETMGR_H_
