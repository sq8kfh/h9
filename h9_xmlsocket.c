#include "h9_xmlsocket.h"
#include "h9_log.h"

#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define STATE_DETECT 0
#define STATE_PREFIX_TRAN 1
#define STATE_STD_TRAN 2
#define STATE_STD_TRAN_COMPLEX_NODE 3

static int prefix_socket_recvxml(h9_xmlsocket_t *xmlsocket, h9_xmlsocket_read_callback_t *callback, void *ud);
static int raw_socket_recvxml(h9_xmlsocket_t *xmlsocket, h9_xmlsocket_read_callback_t *callback, void *ud);
static int sendall(h9_xmlsocket_t *xmlsocket, char *buf, size_t length);
static void *get_in_sockaddr(struct sockaddr *sa);

h9_xmlsocket_t *h9_xmlsocket_init(int socket, size_t init_buf_size) {
    h9_xmlsocket_t *xmlsocket = malloc(sizeof(h9_xmlsocket_t));

    xmlsocket->socket_d = socket;
    xmlsocket->state = STATE_DETECT;
    xmlsocket->in_buf = 0;
    xmlsocket->buf_size = init_buf_size;
    xmlsocket->buf = malloc(xmlsocket->buf_size);
    xmlsocket->msg_size = 0;
    xmlsocket->recv_byte_counter = 0;
    xmlsocket->send_byte_counter = 0;
    return xmlsocket;
}

h9_xmlsocket_t *h9_xmlsocket_connect(const char *hostname, const char *servname, size_t init_buf_size) {
    int sockfd = -1;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, servname, &hints, &servinfo)) != 0) {
        h9_log_err("xmlsocket: getaddrinfo: %s", gai_strerror(rv));
        return NULL;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            h9_log_warn("xmlsocket: socket %s", strerror(errno));
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            h9_log_warn("xmlsocket: connect %s", strerror(errno));
            continue;
        }
        break;
    }

    if (p == NULL) {
        h9_log_err("xmlsocket: failed to connect");
        return NULL;
    }

    inet_ntop(p->ai_family, get_in_sockaddr(p->ai_addr), s, sizeof s);
    h9_log_info("xmlsocket: connecting to %s", s);

    freeaddrinfo(servinfo);

    h9_xmlsocket_t *xmlsocket = h9_xmlsocket_init(sockfd, init_buf_size);
    xmlsocket->state = STATE_PREFIX_TRAN;

    return xmlsocket;
}

void h9_xmlsocket_free(h9_xmlsocket_t *xmlsocket) {
    h9_log_debug("xmlsocket %p stats: recv %u B; send %u B; buf size %u B",
                 xmlsocket,
                 xmlsocket->recv_byte_counter,
                 xmlsocket->send_byte_counter,
                 xmlsocket->buf_size);
    if (xmlsocket->buf) {
        free(xmlsocket->buf);
    }
    close(xmlsocket->socket_d);
    free(xmlsocket);
}

int h9_xmlsocket_recv(h9_xmlsocket_t *xmlsocket, h9_xmlsocket_read_callback_t *callback, void *callback_data) {
    ssize_t nbytes;

    if (xmlsocket->in_buf >= xmlsocket->buf_size) {
        h9_log_debug("xmlsocket: read buffer (%d) is to small - resizing", xmlsocket->buf_size);
        xmlsocket->buf = realloc(xmlsocket->buf, xmlsocket->buf_size * 2);
        xmlsocket->buf_size = xmlsocket->buf_size * 2;

        if (xmlsocket->buf == NULL) {
            h9_log_err("xmlsocket: realloc: %s", strerror(errno));
            return -1;
        }
    }

    nbytes = recv(xmlsocket->socket_d, &xmlsocket->buf[xmlsocket->in_buf], xmlsocket->buf_size - xmlsocket->in_buf, 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            h9_log_debug("xmlsocket: socket %d hung up", xmlsocket->socket_d);
            return 0;
        } else {
            h9_log_err("xmlsocket recv %s", strerror(errno));
            return -1;
        }
    } else {
        xmlsocket->recv_byte_counter += nbytes;
        xmlsocket->in_buf += nbytes;

        if (xmlsocket->state == STATE_DETECT) {
            if (xmlsocket->in_buf >= 7 && strncasecmp(&xmlsocket->buf[4], "<h9", 3) == 0) {
                xmlsocket->state = STATE_PREFIX_TRAN;
            }
            else if (xmlsocket->in_buf >= 7 || (xmlsocket->in_buf >= 3 && strncasecmp(xmlsocket->buf, "<h9", 3) == 0)) {
                xmlsocket->state = STATE_STD_TRAN;
            }
        }

        if (xmlsocket->state == STATE_PREFIX_TRAN) {
            if (prefix_socket_recvxml(xmlsocket, callback, callback_data) <= 0) {
                return 0;
            }
        }
        else if (xmlsocket->state == STATE_STD_TRAN) {
            if (raw_socket_recvxml(xmlsocket, callback, callback_data) <= 0) {
                return 0;
            }
        }
        h9_log_debug("recv state: %d, in buf: %d, msg size: %d",
                     xmlsocket->state, xmlsocket->in_buf, xmlsocket->msg_size);
    }
    return 1;
}

int h9_xmlsocket_send(h9_xmlsocket_t *xmlsocket, const char *msg, size_t length) {
    if (xmlsocket->state == STATE_PREFIX_TRAN) {
        uint32_t length_n = htonl(*((uint32_t *) length));

        if (sendall(xmlsocket, (char *) &length_n, sizeof(length_n)) <= 0) {
            h9_log_err("xmlsocket: send: %s", strerror(errno));
            return 0;
        }
        if (sendall(xmlsocket, (char *) msg, length) <= 0) {
            h9_log_err("xmlsocket: send: %s", strerror(errno));
            return 0;
        }
    }
    else if (xmlsocket->state >= STATE_STD_TRAN) {
        if (sendall(xmlsocket, (char *) msg, length) <= 0) {
            h9_log_err("xmlsocket: send: %s", strerror(errno));
            return 0;
        }
    }

    return 1;
}

static int prefix_socket_recvxml(h9_xmlsocket_t *xmlsocket, h9_xmlsocket_read_callback_t *callback, void *callback_data) {
    while (xmlsocket->in_buf >= 4) {
        if (xmlsocket->msg_size == 0) {
            xmlsocket->msg_size = ntohl(*((uint32_t *) xmlsocket->buf));
        }

        if (xmlsocket->msg_size && xmlsocket->in_buf >= xmlsocket->msg_size + 4) {
            h9_log_debug("recv prefix xml %d: %.*s", xmlsocket->msg_size, xmlsocket->msg_size,
                         &xmlsocket->buf[4]);

            if (callback(xmlsocket->buf, xmlsocket->msg_size, callback_data) <= 0) {
                xmlsocket->msg_size = 0;
                return 0;
            }

            memmove(xmlsocket->buf, &xmlsocket->buf[xmlsocket->msg_size + 4], xmlsocket->in_buf - xmlsocket->msg_size - 4);

            xmlsocket->in_buf = xmlsocket->in_buf - xmlsocket->msg_size - 4;
            xmlsocket->msg_size = 0;
        } else {
            break;
        }
    }
    return 1;
}

static int raw_socket_recvxml(h9_xmlsocket_t *xmlsocket, h9_xmlsocket_read_callback_t *callback, void *callback_data) {
    while (xmlsocket->in_buf >= 3) {
        if (xmlsocket->msg_size == 0) {
            size_t i = 0;
            for (; i < xmlsocket->in_buf - 2; ++i) {
                if (strncasecmp(&xmlsocket->buf[i], "<h9", 3) == 0) {
                    xmlsocket->msg_size = 3;
                    break;
                }
            }
            if (i) {
                memmove(xmlsocket->buf, &xmlsocket->buf[i], xmlsocket->in_buf - i);
                xmlsocket->in_buf = xmlsocket->in_buf - i;
            }
        }
        if (xmlsocket->msg_size && xmlsocket->msg_size < xmlsocket->in_buf) {
            for (size_t i = xmlsocket->msg_size; i < xmlsocket->in_buf; ++i) {
                if (xmlsocket->state == STATE_STD_TRAN && xmlsocket->buf[i] == '>') {
                    if (xmlsocket->buf[i-1] == '/') {
                        xmlsocket->msg_size = i + 1;
                        h9_log_debug("recv short xml %d: %.*s", xmlsocket->msg_size,
                                     xmlsocket->msg_size,
                                     xmlsocket->buf);

                        if (callback(xmlsocket->buf, xmlsocket->msg_size, callback_data) <= 0) {
                            xmlsocket->msg_size = 0;
                            return 0;
                        }

                        memmove(xmlsocket->buf, &xmlsocket->buf[xmlsocket->msg_size],
                                xmlsocket->in_buf - xmlsocket->msg_size);

                        xmlsocket->in_buf = xmlsocket->in_buf - xmlsocket->msg_size;
                        xmlsocket->msg_size = 0;
                        break;
                    }
                    else {
                        xmlsocket->state = STATE_STD_TRAN_COMPLEX_NODE;
                    }
                }
                if (i < xmlsocket->in_buf - 3 && strncasecmp(&xmlsocket->buf[i], "</h9", 4) == 0) {
                    for (size_t j = i; j < xmlsocket->in_buf; ++j) {
                        if (xmlsocket->buf[j] == '>') {
                            xmlsocket->state = STATE_STD_TRAN;
                            xmlsocket->msg_size = xmlsocket->msg_size + (j - i + 1);
                            h9_log_debug("recv xml 2 %d: %.*s", xmlsocket->msg_size,
                                         xmlsocket->msg_size,
                                         xmlsocket->buf);

                            if (callback(xmlsocket->buf, xmlsocket->msg_size, callback_data) <= 0) {
                                xmlsocket->msg_size = 0;
                                return 0;
                            }

                            memmove(xmlsocket->buf, &xmlsocket->buf[xmlsocket->msg_size],
                                    xmlsocket->in_buf - xmlsocket->msg_size);

                            xmlsocket->in_buf = xmlsocket->in_buf - xmlsocket->msg_size;
                            xmlsocket->msg_size = 0;
                            break;
                        }
                    }
                } else {
                    xmlsocket->msg_size++;
                }
            }
        }
        else {
            break;
        }
    }
    return 1;
}

static int sendall(h9_xmlsocket_t *xmlsocket, char *buf, size_t length) {
    size_t total = 0;
    size_t bytesleft = length;
    ssize_t n = 0;

    while (total < length) {
        n = send(xmlsocket->socket_d, buf+total, bytesleft, 0);
        if (n == -1) {
            break;
        }
        total += n;
        bytesleft -= n;
    }
    xmlsocket->send_byte_counter += total;
    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

// get sockaddr, IPv4 or IPv6:
static void *get_in_sockaddr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
