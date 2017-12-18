#include "h9_slcan.h"
#include "h9_log.h"
#include "h9d_endpoint.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>

static int send_first(h9_slcan_t *slcan);
static int send_ack(h9_slcan_t *slcan, onselect_callback_t *send_callback, void *callback_data);
static char *strndup_unescaped(const char *ptr, size_t n);
static size_t build_msg(char *slcan_data, const h9msg_t *msg);
static int proces_readed_data(h9_slcan_t *slcan, const char *data, size_t length,
                              onselect_callback_t *callback, void *callback_data);

typedef struct queue_t {
    struct queue_t *next;
    char msg[30];
    size_t length;
} queue_t;

queue_t *msg_queue_start;
queue_t *msg_queue_end;

h9_slcan_t *h9_slcan_connect(const char *connect_string, size_t init_buf_size) {
    msg_queue_start = NULL;
    msg_queue_end = NULL;

    int fd = open(connect_string, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        h9_log_err("open tty: %s", strerror(errno));
        return NULL;
    }
    else {
        fcntl(fd, F_SETFL, 0);
    }

    struct termios options;

    tcgetattr(fd, &options);

    cfsetispeed(&options, B230400);
    cfsetospeed(&options, B230400);

    options.c_cflag |= (CLOCAL | CREAD); /* Enable the receiver and set local mode */
    /*
     * Select 8N1
     */
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_cflag &= ~CRTSCTS; /* Disable hardware flow control */
    options.c_iflag &= ~(IXON | IXOFF | IXANY); /* Disable software flow control */

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /* Raw Input */

    options.c_oflag &= ~OPOST; /* Raw Output */

    //options.c_cc[VMIN]  = 0;
    //options.c_cc[VTIME] = 10;

    tcsetattr(fd, TCSANOW, &options);

    h9_slcan_t *sl = malloc(sizeof(h9_slcan_t));

    sl->fd = fd;

    sl->in_buf = 0;
    sl->buf_size = init_buf_size;
    sl->buf = malloc(sl->buf_size);
    sl->buf_ptr = 0;

    sl->write_byte_counter = 0;
    sl->read_byte_counter = 0;

    return sl;
}

void h9_slcan_free(h9_slcan_t *slcan) {
    h9_log_debug("slcan %p stats: read %u B; write %u B",
                 slcan,
                 slcan->read_byte_counter,
                 slcan->write_byte_counter);
    free(slcan->buf);
    free(slcan);
}

int h9_slcan_onselect_event(h9_slcan_t *slcan,
                            onselect_callback_t *recv_callback,
                            onselect_callback_t *send_callback,
                            void *callback_data) {
    ssize_t nbytes;

    if (slcan->in_buf >= slcan->buf_size) {
        h9_log_debug("slcan: read buffer (%d) is to small - resizing", slcan->buf_size);
        slcan->buf = realloc(slcan->buf, slcan->buf_size * 2);
        slcan->buf_size = slcan->buf_size * 2;

        if (slcan->buf == NULL) {
            h9_log_err("slcan: realloc: %s", strerror(errno));
            return ONSELECT_CRITICAL;
        }
    }

    nbytes = read(slcan->fd, &slcan->buf[slcan->in_buf], slcan->buf_size - slcan->in_buf);

    int ret = ONSELECT_SUCCESS;

    if (nbytes <= 0) {
        if (nbytes == 0) {
            h9_log_err("slcan closed fd");
        } else {
            h9_log_err("slcan read %s", strerror(errno));
        }
        return ONSELECT_CRITICAL;
    } else {
        slcan->read_byte_counter += nbytes;
        slcan->in_buf += nbytes;

        while (slcan->buf_ptr < slcan->in_buf) {
            size_t i;
            for (i = slcan->buf_ptr; i < slcan->in_buf; ++i) {
                if (slcan->buf[i] == '\r' || slcan->buf[i] == '\a') {
                    if (i > 0) { //length > 1 - message
                        if (!proces_readed_data(slcan, slcan->buf, i + 1, recv_callback, callback_data)) {
                            char *tmp = strndup_unescaped(slcan->buf, i + 1);
                            h9_log_warn("slcan read invalid %d: '%s'", i + 1, tmp);
                            free(tmp);
                        }
                        else {
                            char *tmp = strndup_unescaped(slcan->buf, i + 1);
                            h9_log_debug("slcan read %d: '%s'", i + 1, tmp);
                            free(tmp);
                        }
                    }
                    else { //ack, nack
                        h9_log_debug("slcan read: %s", slcan->buf[i] == '\r' ? "ACK" : "NACK");
                        ret = send_ack(slcan, send_callback, callback_data);
                    }
                    memmove(slcan->buf, &slcan->buf[i + 1],
                            slcan->in_buf - i - 1);

                    slcan->in_buf = slcan->in_buf - i - 1;
                    i = 0;

                    break;
                }
            }
            slcan->buf_ptr = i;
        }
    }
    return ret;
}

int h9_slcan_send(h9_slcan_t *slcan, const h9msg_t *msg) {
    queue_t *q = malloc(sizeof(queue_t));
    q->length = build_msg(q->msg, msg);
    q->next = NULL;

    int ret = ONSELECT_SUCCESS;

    if (msg_queue_start == NULL) {
        msg_queue_start = q;
        msg_queue_end = q;

        ret = send_first(slcan);
    }
    else {
        msg_queue_end->next = q;
        msg_queue_end = q;
    }

    return ret;
}

static int send_first(h9_slcan_t *slcan) {
    if (msg_queue_start != NULL) {
        ssize_t nbyte = write(slcan->fd, msg_queue_start->msg, msg_queue_start->length);
        if (nbyte <= 0) {
            if (nbyte == 0) {
                h9_log_err("slcan closed fd");
            } else {
                h9_log_err("slcan write %s", strerror(errno));
            }
            return ONSELECT_CRITICAL;
        }

        char *tmp = strndup_unescaped(msg_queue_start->msg, nbyte);
        h9_log_debug("slcan write %d: '%s'", nbyte, tmp);
        free(tmp);

        slcan->write_byte_counter += nbyte;
    }
    return ONSELECT_SUCCESS;
}

static int send_ack(h9_slcan_t *slcan, onselect_callback_t *send_callback, void *callback_data) {
    if (msg_queue_start != NULL) {
        queue_t *q = msg_queue_start;
        msg_queue_start = msg_queue_start->next;
        if (q == msg_queue_end) {
            msg_queue_end = NULL;
        }

        send_callback(NULL, callback_data);

        free(q);
    }
    return send_first(slcan);
}

static char *strndup_unescaped(const char *ptr, size_t n) {
    if (!ptr) return NULL;
    char *ret = malloc(n*2);
    char *p = ret;
    for (int i = 0; i < n; i++, p++) {
        switch (ptr[i]) {
            case '\0': *p++ = '\\'; *p = '0'; break;
            case '\a': *p++ = '\\'; *p = 'a'; break;
            case '\b': *p++ = '\\'; *p = 'b'; break;
            case '\f': *p++ = '\\'; *p = 'f'; break;
            case '\n': *p++ = '\\'; *p = 'n'; break;
            case '\r': *p++ = '\\'; *p = 'r'; break;
            case '\t': *p++ = '\\'; *p = 't'; break;
            case '\v': *p++ = '\\'; *p = 'v'; break;
            case '\\': *p++ = '\\'; *p = '\\'; break;
            case '\?': *p++ = '\\'; *p = '?'; break;
            case '\'': *p++ = '\\'; *p = '\''; break;
            case '\"': *p++ = '\\'; *p = '"'; break;
            default: *p = ptr[i]; break;
        }
    }
    *p = '\0';
    return ret;
}

static size_t build_msg(char *slcan_data, const h9msg_t *msg) {
    uint32_t id = 0;
    id |= msg->priority & ((1<<H9_MSG_PRIORITY_BIT_LENGTH) - 1);
    id <<= H9_MSG_TYPE_BIT_LENGTH;
    id |= msg->type & ((1<<H9_MSG_TYPE_BIT_LENGTH) - 1);
    id <<= H9_MSG_RESERVED_BIT_LENGTH;
    id |= H9_MSG_RESERVED_VALUE & ((1<<H9_MSG_RESERVED_BIT_LENGTH) - 1);
    id <<= H9_MSG_DESTINATION_ID_BIT_LENGTH;
    id |= msg->destination_id & ((1<<H9_MSG_DESTINATION_ID_BIT_LENGTH) - 1);
    id <<= H9_MSG_SOURCE_ID_BIT_LENGTH;
    id |= msg->source_id & ((1<<H9_MSG_SOURCE_ID_BIT_LENGTH) - 1);

    size_t nbyte;
    nbyte = sprintf(slcan_data, "T%08X%1hhu", id, msg->dlc);
    for (int i = 0; i < msg->dlc; ++i) {
        nbyte += sprintf(slcan_data + nbyte, "%02hhX", msg->data[i]);
    }
    nbyte += sprintf(slcan_data + nbyte, "\r");

    return nbyte;
}

static h9msg_t *parse_msg(const char *data, size_t length) {
    h9msg_t *res = h9msg_init();
    uint32_t id;
    sscanf(data + 1, "%8x", &id);

    res->priority = (uint8_t)((id >> (H9_MSG_TYPE_BIT_LENGTH + H9_MSG_RESERVED_BIT_LENGTH +
                                     H9_MSG_DESTINATION_ID_BIT_LENGTH + H9_MSG_SOURCE_ID_BIT_LENGTH)) & ((1<<H9_MSG_PRIORITY_BIT_LENGTH) - 1));

    res->type = (uint8_t)((id >> (H9_MSG_RESERVED_BIT_LENGTH + H9_MSG_DESTINATION_ID_BIT_LENGTH + H9_MSG_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9_MSG_TYPE_BIT_LENGTH) - 1));

    res->destination_id = (uint16_t)((id >> (H9_MSG_SOURCE_ID_BIT_LENGTH)) & ((1<<H9_MSG_DESTINATION_ID_BIT_LENGTH) - 1));

    res->source_id = (uint16_t)((id >> (0)) & ((1<<H9_MSG_SOURCE_ID_BIT_LENGTH) - 1));

    uint32_t dlc;

    sscanf(data + 9, "%1u", &dlc);

    res->dlc = (uint8_t)dlc;
    for (int i=0; i < dlc; ++i) {
        unsigned int tmp;
        sscanf(data + 10 + i*2, "%2x", &tmp);
        res->data[i] = (uint8_t)tmp;
    }

    return res;
}

static int proces_readed_data(h9_slcan_t *slcan, const char *data, size_t length,
                              onselect_callback_t *callback, void *callback_data) {
    int res = 0;
    if (data[0] == 't' && length <= 22) {
        h9_log_warn("slcan t command not yet implemented");
    }
    else if (data[0] == 'T' && length <= 27) {
        h9msg_t *msg = parse_msg(data, length);
        callback(msg, callback_data);
        h9msg_free(msg);
        res = 1;
    }
    else if (data[0] == 'r' && length <= 6) {
        h9_log_warn("slcan r command not yet implemented");
    }
    else if (data[0] == 'R' && length <= 11) {
        h9_log_warn("slcan R command not yet implemented");
    }
    else if (data[0] == 'F' && length == 6) {
        h9_log_warn("slcan F command not yet implemented");
    }
    else if (data[0] == 'v' && length == 6) {
        h9_log_warn("slcan v command not yet implemented");
    }
    else if (data[0] == 'V' && length == 6) {
        h9_log_warn("slcan V command not yet implemented");
    }
    else if (data[0] == 'N' && length == 6) {
        h9_log_warn("slcan N command not yet implemented");
    }
    else {
        h9_log_err("slcan unknown command %c", data[0]);
    }
    if (res == 0) {
        callback(NULL, callback_data);
    }
    return res;
}
