#include "h9d_client_module.h"
#include "h9d_select_event.h"
#include "h9_xmlmsg.h"
#include "h9_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/socket.h>

#define INIT_BUFFER_SIZE 100

#define STATE_INIT 0
#define STATE_PREFIX_TRAN 1
#define STATE_STD_TRAN 2

h9d_client_module_t *h9d_client_module_init(int socket) {
    h9d_client_module_t *mc = malloc(sizeof(h9d_client_module_t));

    mc->socket_d = socket;
    mc->state = STATE_INIT;
    mc->in_buf = 0;
    mc->buf_size = INIT_BUFFER_SIZE + 1;
    mc->buf = malloc(mc->buf_size);
    mc->msg_size = 0;
    return mc;
}

void h9d_client_module_free(h9d_client_module_t *cm) {
    free(cm->buf);
    free(cm);
}

int h9d_client_module_process_xmlmsg(char *msg, size_t msg_size) {
    int ret = h9_xmlmsg_pare(msg, msg_size, 1);

    return 1;
}

int h9d_client_module_process_events(h9d_client_module_t *ev_data, int event_type, time_t elapsed) {
    if (event_type == H9D_SELECT_EVENT_READ) {
        int nbytes;

        if (ev_data->in_buf >= ev_data->buf_size) {
            h9_log_debug("%s: read buffer (%d) is to small - resizing\n", __func__, ev_data->buf_size);
            ev_data->buf = realloc(ev_data->buf, ev_data->buf_size * 2 + 1);
            ev_data->buf_size = ev_data->buf_size * 2;

            if (ev_data->buf == NULL) {
                perror("realloc");
            }
        }

        nbytes = recv(ev_data->socket_d, &ev_data->buf[ev_data->in_buf], ev_data->buf_size - ev_data->in_buf, 0);

        if (nbytes <= 0) {
            if (nbytes == 0) {
                printf("selectserver: socket %d hung up\n", ev_data->socket_d);
            } else {
                perror("recv");
            }

            close(ev_data->socket_d);
            h9d_client_module_free(ev_data);

            return H9D_SELECT_EVENT_RETURN_DEL;
        } else {
            ev_data->in_buf += nbytes;

            if (ev_data->state == STATE_INIT) {
                if (ev_data->in_buf >= 7 && strncasecmp(&ev_data->buf[4], "<h9", 3) == 0) {
                    ev_data->state = STATE_PREFIX_TRAN;
                }
                else if (ev_data->in_buf >= 7 || (ev_data->in_buf >= 3 && strncasecmp(ev_data->buf, "<h9", 3) == 0)) {
                    ev_data->state = STATE_STD_TRAN;
                }
            }

            if (ev_data->state == STATE_PREFIX_TRAN) {
                while (ev_data->in_buf >= 4) {
                    if (ev_data->msg_size == 0) {
                        ev_data->msg_size = ntohl(*((uint32_t *) ev_data->buf));
                    }

                    if (ev_data->msg_size && ev_data->in_buf >= ev_data->msg_size + 4) {
                        h9_log_debug("%s: recv %d: %.*s\n", __func__, ev_data->msg_size, ev_data->msg_size,
                                     &ev_data->buf[4]);

                        if (h9d_client_module_process_xmlmsg(&ev_data->buf[4], ev_data->msg_size) == 0) {
                            return H9D_SELECT_EVENT_RETURN_DEL;
                        }

                        memmove(ev_data->buf, &ev_data->buf[ev_data->msg_size + 4], ev_data->in_buf - ev_data->msg_size - 4);

                        ev_data->in_buf = ev_data->in_buf - ev_data->msg_size - 4;
                        ev_data->msg_size = 0;
                    } else {
                        break;
                    }
                }
            }
            else if (ev_data->state == STATE_STD_TRAN) {
                while (ev_data->in_buf >= 3) {
                    if (ev_data->msg_size == 0) {
                        size_t i = 0;
                        int con_flag = 0;
                        for (; i < ev_data->in_buf - 3; ++i) {
                            if (strncasecmp(&ev_data->buf[i], "<h9", 3) == 0) {
                                ev_data->msg_size = 3;
                                for (size_t j = i + 3; j < ev_data->in_buf-1; ++j) {
                                    if (ev_data->buf[j] == '>') {
                                        break;
                                    }
                                    else if (ev_data->buf[j] == '/' && ev_data->buf[j+1] == '>') {
                                        ev_data->msg_size = ev_data->msg_size + (j - i - 1);
                                        h9_log_debug("%s: recv3 %d: %.*s\n", __func__, ev_data->msg_size,
                                                     ev_data->msg_size,
                                                     ev_data->buf);

                                        if (h9d_client_module_process_xmlmsg(ev_data->buf, ev_data->msg_size) == 0) {
                                            return H9D_SELECT_EVENT_RETURN_DEL;
                                        }

                                        memmove(ev_data->buf, &ev_data->buf[ev_data->msg_size],
                                                ev_data->in_buf - ev_data->msg_size);

                                        ev_data->in_buf = ev_data->in_buf - ev_data->msg_size;
                                        ev_data->msg_size = 0;
                                        con_flag = 1;
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        if (con_flag) {
                            continue;
                        }
                        if (ev_data->msg_size && i) {
                            memmove(ev_data->buf, &ev_data->buf[i], ev_data->in_buf - i);
                            ev_data->in_buf = ev_data->in_buf - i;
                        }
                    }
                    if (ev_data->msg_size) {
                        for (size_t i = ev_data->msg_size; i < ev_data->in_buf - 4; ++i) {
                            if (strncasecmp(&ev_data->buf[i], "</h9", 4) == 0) {
                                for (size_t j = i; j < ev_data->in_buf; ++j) {
                                    if (ev_data->buf[j] == '>') {
                                        ev_data->msg_size = ev_data->msg_size + (j - i + 1);
                                        h9_log_debug("%s: recv2 %d: %.*s\n", __func__, ev_data->msg_size,
                                                     ev_data->msg_size,
                                                     ev_data->buf);

                                        if (h9d_client_module_process_xmlmsg(ev_data->buf, ev_data->msg_size) == 0) {
                                            return H9D_SELECT_EVENT_RETURN_DEL;
                                        }

                                        memmove(ev_data->buf, &ev_data->buf[ev_data->msg_size],
                                                ev_data->in_buf - ev_data->msg_size);

                                        ev_data->in_buf = ev_data->in_buf - ev_data->msg_size;
                                        ev_data->msg_size = 0;
                                        break;
                                    }
                                }
                                if (ev_data->msg_size == 0) {
                                    break;
                                }
                            } else {
                                ev_data->msg_size++;
                            }
                        }
                    }
                    else {
                        break;
                    }
                }
            }
            h9_log_debug("%s: recv state: %d, in buf: %d, msg size: %d\n", __func__,
                         ev_data->state, ev_data->in_buf, ev_data->msg_size);
        }
    }

    return H9D_SELECT_EVENT_RETURN_OK;
}
