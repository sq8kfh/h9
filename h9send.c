#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "h9_log.h"
#include "h9_xmlsocket.h"
#include "h9_xmlmsg.h"

static void help(void) {
    h9_log_stderr("usage: h9spy [-dhvV] [-c host] [-p port] [-P H|L] [-t type] [-s id] [-i id] <data>");
    h9_log_stderr("");
    h9_log_stderr("Options:");
    h9_log_stderr("   -c <host>  --connect=<host>");
    h9_log_stderr("   -d,        --debug");
    h9_log_stderr("   -h,        --help");
    h9_log_stderr("   -i <id>    --destination=<id>");
    h9_log_stderr("   -p <port>  --port=<port>");
    h9_log_stderr("   -P H|L     --priority=H|L");
    h9_log_stderr("   -s <id>    --source=<id>");
    h9_log_stderr("   -t <type>  --type=<type>");
    h9_log_stderr("   -v,        --verbose");
    h9_log_stderr("   -V,        --Version");
    exit(EXIT_FAILURE);
}

static void usage(void) {
    h9_log_stderr("usage: h9spy [-dhvV] [-c host] [-p port] [-P H|L] [-t type] [-s id] [-i id] <data>");
    exit(EXIT_FAILURE);
}

static void version(void) {
    h9_log_stderr("h9send version %s by SQ8KFH", H9D_VERSION);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int verbose = H9_LOG_STDERR;
    int debug = 0;

    h9msg_t *msg = h9msg_init();

    int c;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"connect",     required_argument, 0, 'c' },
                {"debug",       no_argument,       0, 'd' },
                {"help",        no_argument,       0, 'h' },
                {"port",        required_argument, 0, 'p' },
                {"verbose",     no_argument,       0, 'v' },
                {"version",     no_argument,       0, 'V' },
                {"priority",    required_argument, 0, 'P' },
                {"type",        required_argument, 0, 't' },
                {"destination", required_argument, 0, 'i' },
                {"source",      required_argument, 0, 's' },
                {0,             0,                 0,  0  }
        };

        c = getopt_long(argc, argv, "c:dhp:vVP:t:i:s:", long_options, &option_index);
        if (c == -1)
            break;

        uint16_t tmp_ui;
        char tmp_ch;

        switch (c) {
            /*case 0:
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                usage();
                break;*/
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
            case 'P':
                sscanf(optarg, "%c", &tmp_ch);
                if (tmp_ch == 'H' || tmp_ch == 'h')
                    msg->priority = H9_MSG_PRIORITY_HIGH;
                else
                    msg->priority = H9_MSG_PRIORITY_LOW;
                break;
            case 't':
                sscanf(optarg, "%hu", &tmp_ui);
                msg->type = (uint8_t)tmp_ui & (uint8_t)((1<<H9_MSG_TYPE_BIT_LENGTH) - 1);
                break;
            case 'i':
                sscanf(optarg, "%hu", &tmp_ui);
                msg->destination_id = tmp_ui & (uint16_t)((1<<H9_MSG_DESTINATION_ID_BIT_LENGTH) - 1);
                break;
            case 's':
                sscanf(optarg, "%hu", &tmp_ui);
                msg->source_id = tmp_ui & (uint16_t)((1<<H9_MSG_SOURCE_ID_BIT_LENGTH) - 1);
                break;
            default:
                usage();
                break;
        }
    }

    if (optind < argc) {
        uint8_t tmp_ui;
        uint8_t i = 0;
        for (;i < 8 && optind < argc; ++i) {
            sscanf(argv[optind++], "%hhx", &tmp_ui);
            msg->data[i] = tmp_ui;
        }
        msg->dlc = i;
        if (optind < argc) {
            h9_log_stderr("Too much data, dlc can be max 8");
            h9msg_free(msg);
            return EXIT_FAILURE;
        }
    }

    h9_log_init(verbose, debug, 1);

    size_t length;
    char *xmlmsg = h9_xmlmsg_build_h9sendmsg(&length, msg, 1);

    h9msg_free(msg);

    if (!xmlmsg) {
        h9_log_stderr("Incorrect message");
        return EXIT_FAILURE;
    }

    h9_xmlsocket_t *xmlsocket = h9_xmlsocket_connect("127.0.0.1", "7878", 100);
    if (! xmlsocket) {
        h9_log_stderr("Connection error");
        return EXIT_FAILURE;
    }

    if (!h9_xmlsocket_send(xmlsocket, xmlmsg, length)) {
        h9_log_stderr("Send error");
        h9_xmlsocket_free(xmlsocket);
        return EXIT_FAILURE;
    }

    h9_xmlsocket_free(xmlsocket);
    return EXIT_SUCCESS;
}
