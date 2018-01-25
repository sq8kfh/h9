#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>

#include "h9_log.h"
#include "h9_xmlsocket.h"
#include "h9_xmlmsg.h"

uint16_t dst_id;
h9_xmlsocket_t *xmlsocket;
uint8_t *fw = NULL;
size_t fw_size;

static void help(void) {
    h9_log_stderr("usage: h9fupgrade [-bdhvV] [-c host] [-p port] [-i id] <ihex file>");
    h9_log_stderr("");
    h9_log_stderr("Options:");
    h9_log_stderr("   -b,        --noupgrademsg");
    h9_log_stderr("   -c <host>  --connect=<host>");
    h9_log_stderr("   -d,        --debug");
    h9_log_stderr("   -h,        --help");
    h9_log_stderr("   -i <id>    --destination=<id>");
    h9_log_stderr("   -p <port>  --port=<port>");
    h9_log_stderr("   -v,        --verbose");
    h9_log_stderr("   -V,        --Version");
    exit(EXIT_FAILURE);
}

static void usage(void) {
    h9_log_stderr("usage: h9fupgrade [-bdhvV] [-c host] [-p port] [-i id] <ihex file>");
    exit(EXIT_FAILURE);
}

static void version(void) {
    h9_log_stderr("h9fupgrade version %s by SQ8KFH", H9D_VERSION);
    exit(EXIT_FAILURE);
}

uint32_t hex2bin(const char *hex, const size_t size) {
    uint32_t val = 0;
    const size_t _size = size * 2;
    for (size_t n = 0; n < _size; n++) {
        char c = hex[n];
        if (c == '\0')
            break;

        if (c >= '0' && c <= '9')
            c = c - '0';
        else if (c >= 'a' && c <='f')
            c = c - 'a' + 10;
        else if (c >= 'A' && c <='F')
            c = c - 'A' + 10;

        val = (val << 4) | (c & 0xf);
    }

    return val;
}

static size_t read_ihex(char *ihex_file, uint8_t **fw_data) {
    *fw_data = malloc(128*1024);
    bzero(*fw_data, 128*1024);

    size_t ret = 0;
    FILE *fp = fopen(ihex_file, "r");
    if (fp == NULL) {
        h9_log_stderr("Failed to open file: %s", ihex_file);
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) > 0) {
        if (line[0] != ':')
            continue;

        uint8_t size = (uint8_t)hex2bin(line + 1, 1);
        //uint16_t addr = (uint16_t)hex2bin(line + 3, 2);
        uint8_t rtype = (uint8_t )hex2bin(line + 7, 1);

        if (rtype == 0) {
            for (size_t n = 0; n < size; n++) {
                (*fw_data)[ret++] = (uint8_t) hex2bin(line + 9 + n*2, 1);
            }
        }
    }
    fclose(fp);
    if (line) {
        free(line);
    }
    return ret;
}

static void send_msg(h9_xmlsocket_t *xmlsocket, h9msg_t *msg) {
    size_t length;
    char *xmlmsg = h9_xmlmsg_build_h9sendmsg(&length, msg, 1);

    h9msg_free(msg);

    if (!xmlmsg) {
        h9_log_stderr("Incorrect message");
        exit(EXIT_FAILURE);
    }

    if (!h9_xmlsocket_send(xmlsocket, xmlmsg, length)) {
        h9_log_stderr("Send error");
        h9_xmlsocket_free(xmlsocket);
        exit(EXIT_FAILURE);
    }

    free(xmlmsg);
}


uint16_t page = 0;
size_t fw_idx = 0;

static unsigned int xmlsocket_read_callback(const char *msg, size_t length, void *ud) {
    void *res;
    int ret = h9_xmlmsg_parse(msg, length, &res, 1);

    if (ret == H9_XMLMSG_MSG && res) {
        h9msg_t *msg = (h9msg_t*)res;
        if (msg->type == H9MSG_TYPE_ENTER_INTO_BOOTLOADER || msg->type == H9MSG_TYPE_PAGE_WRITED) {
            if (msg->type == H9MSG_TYPE_PAGE_WRITED) {
                h9_log_stderr("Writted page %hu, byte %u", page, fw_idx);
                if (fw_idx >= fw_size) {
                    h9msg_t *m = h9msg_init();
                    m->source_id = 10;
                    m->destination_id = dst_id;
                    m->type = H9MSG_TYPE_QUIT_BOOTLOADER;
                    send_msg(xmlsocket, m);

                    exit(EXIT_SUCCESS);
                }
                page++;
            }
            h9msg_t *m = h9msg_init();
            m->source_id = 10;
            m->destination_id = dst_id;
            m->type = H9MSG_TYPE_PAGE_START;
            m->dlc = 2;
            m->data[0] = (uint8_t)((page >> 8) & 0xff);
            m->data[1] = (uint8_t)((page) & 0xff);
            send_msg(xmlsocket, m);
        }
        else if (msg->type == H9MSG_TYPE_PAGE_FILL_NEXT) {
            h9msg_t *m = h9msg_init();
            m->source_id = 10;
            m->destination_id = dst_id;
            m->type = H9MSG_TYPE_PAGE_FILL;
            m->dlc = 8;
            m->data[0] = fw[fw_idx++];
            m->data[1] = fw[fw_idx++];
            m->data[2] = fw[fw_idx++];
            m->data[3] = fw[fw_idx++];
            m->data[4] = fw[fw_idx++];
            m->data[5] = fw[fw_idx++];
            m->data[6] = fw[fw_idx++];
            m->data[7] = fw[fw_idx++];
            send_msg(xmlsocket, m);
        }

        h9msg_free(msg);
    }

    return 1;
}

int main(int argc, char **argv) {
    int verbose = H9_LOG_STDERR;
    int debug = 0;
    int noupgrademsg = 0;
    int c;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"noupgrademsg", no_argument,       0, 'b' },
                {"connect",      required_argument, 0, 'c' },
                {"debug",        no_argument,       0, 'd' },
                {"help",         no_argument,       0, 'h' },
                {"port",         required_argument, 0, 'p' },
                {"verbose",      no_argument,       0, 'v' },
                {"version",      no_argument,       0, 'V' },
                {"destination",  required_argument, 0, 'i' },
                {0,              0,                 0,  0  }
        };

        c = getopt_long(argc, argv, "bc:dhp:vVi:", long_options, &option_index);
        if (c == -1)
            break;

        char tmp_ch;

        switch (c) {
            /*case 0:
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                usage();
                break;*/
            case 'b':
                noupgrademsg = 1;
                break;
            case 'c':
                //host = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            case 'h':
                help();
                break;
            case 'p':
                //port = optarg;
                break;
            case 'v':
                if (!verbose) verbose = H9_LOG_WARN;
                ++verbose;
                break;
            case 'V':
                version();
                break;
            case 'i':
                sscanf(optarg, "%hu", &dst_id);
                dst_id = dst_id & (uint16_t)((1<<H9MSG_DESTINATION_ID_BIT_LENGTH) - 1);
                break;
            default:
                usage();
                break;
        }
    }

    if (optind >= argc) {
        usage();
    }

    h9_log_init(verbose, debug, 1);


    fw_size = read_ihex(argv[optind++], &fw);
    h9_log_stderr("read: %u", fw_size);


    xmlsocket = h9_xmlsocket_connect("127.0.0.1", "7878", 100);
    if (! xmlsocket) {
        h9_log_stderr("Connection error");
        return EXIT_FAILURE;
    }

    size_t length;
    char *msg = h9_xmlmsg_build_h9subscribe(&length, "msg", 1);
    h9_xmlsocket_send(xmlsocket, msg, length);

    h9msg_t *m = h9msg_init();
    m->source_id = 10;
    m->destination_id = dst_id;

    if (noupgrademsg) {
        m->type = H9MSG_TYPE_PAGE_START;
        m->dlc = 2;
        m->data[0] = (uint8_t)((page >> 8) & 0xff);
        m->data[1] = (uint8_t)((page) & 0xff);
    }
    else {
        m->type = H9MSG_TYPE_NODE_UPGRADE;
    }
    send_msg(xmlsocket, m);

    while (1) {
        if (h9_xmlsocket_recv(xmlsocket, xmlsocket_read_callback, NULL) < 0) {
            h9_log_stderr("Disconnected");
            h9_xmlsocket_free(xmlsocket);
            return EXIT_FAILURE;
        }
    }

    h9_xmlsocket_free(xmlsocket);
    return EXIT_SUCCESS;
}
