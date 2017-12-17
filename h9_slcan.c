#include "h9_slcan.h"
#include "h9_log.h"
#include "h9d_endpoint.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <termios.h>

static int proces_readed_data(h9_slcan_t *slcan, const char *data, size_t length,
                              h9_slcan_recv_callback_t *callback, void *callback_data);

h9_slcan_t *h9_slcan_connect(const char *connect_string, size_t init_buf_size) {
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

int h9_slcan_recv(h9_slcan_t *slcan, h9_slcan_recv_callback_t *callback, void *callback_data) {
    ssize_t nbytes;

    if (slcan->in_buf >= slcan->buf_size) {
        h9_log_debug("slcan: read buffer (%d) is to small - resizing", slcan->buf_size);
        slcan->buf = realloc(slcan->buf, slcan->buf_size * 2);
        slcan->buf_size = slcan->buf_size * 2;

        if (slcan->buf == NULL) {
            h9_log_err("slcan: realloc: %s", strerror(errno));
            return -1;
        }
    }

    nbytes = read(slcan->fd, &slcan->buf[slcan->in_buf], slcan->buf_size - slcan->in_buf);

    int ret = 1;

    if (nbytes <= 0) {
        if (nbytes == 0) {
            h9_log_err("slcan closed fd");
        } else {
            h9_log_err("slcan read %s", strerror(errno));
        }
        return -1;
    } else {
        slcan->read_byte_counter += nbytes;
        slcan->in_buf += nbytes;

        for (; slcan->buf_ptr < slcan->in_buf; ++slcan->buf_ptr) {
            if (slcan->buf[slcan->buf_ptr] == '\r' || slcan->buf[slcan->buf_ptr] == '\a') {
                h9_log_debug("slcan read %d: '%.*s'", slcan->buf_ptr+1,
                             slcan->buf_ptr,
                             slcan->buf);

                int res = proces_readed_data(slcan, slcan->buf, slcan->buf_ptr + 1, callback, callback_data);
                if (res <= 0) { // - 1 bad msg, callback no exec; 0 callbac return error
                    ret = 0;
                }

                memmove(slcan->buf, &slcan->buf[slcan->buf_ptr+1],
                        slcan->in_buf - slcan->buf_ptr - 1);

                slcan->in_buf = slcan->in_buf - slcan->buf_ptr - 1;
                slcan->buf_ptr = 0;
            }
        }
    }
    return ret;
}

int h9_slcan_send(h9_slcan_t *slcan, const h9msg_t *msg) {
    return 1;
}

static h9msg_t *parse_msg(const char *data, size_t length) {
    h9msg_t *res = h9msg_init();
    uint32_t id;
    sscanf(data + 1, "%8x", &id);

    res->priority = (uint8_t)(id >> (H9_MSG_TYPE_BIT_LENGTH + H9_MSG_RESERVED_BIT_LENGTH +
                                     H9_MSG_DESTINATION_ID_BIT_LENGTH + H9_MSG_SOURCE_ID_BIT_LENGTH)) & ((1<<H9_MSG_PRIORITY_BIT_LENGTH) - 1);

    res->type = (uint8_t)(id >> (H9_MSG_RESERVED_BIT_LENGTH + H9_MSG_DESTINATION_ID_BIT_LENGTH + H9_MSG_SOURCE_ID_BIT_LENGTH)) \
		                    & ((1<<H9_MSG_TYPE_BIT_LENGTH) - 1);

    res->destination_id = (uint16_t)(id >> (H9_MSG_SOURCE_ID_BIT_LENGTH)) & ((1<<H9_MSG_DESTINATION_ID_BIT_LENGTH) - 1);

    res->source_id = (uint16_t)(id >> (0)) & ((1<<H9_MSG_SOURCE_ID_BIT_LENGTH) - 1);

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
                              h9_slcan_recv_callback_t *callback, void *callback_data) {
    int res = -1;
    if (data[0] == '\r' && length == 1) {
        res = 0;
    }
    else if (data[0] == '\a' && length == 1) {
        res = 0;
    }
    else if (data[0] == 't' && length <= 22) {
        h9_log_warn("slcan t command not yet implemented");
    }
    else if (data[0] == 'T' && length <= 27) {
        h9msg_t *msg = parse_msg(data, length);
        res = callback(msg, callback_data);
        h9msg_free(msg);
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
    return res;
}
