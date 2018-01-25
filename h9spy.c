#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>

#include "h9_log.h"
#include "h9_xmlsocket.h"
#include "h9_xmlmsg.h"

static int run = 1;
static int extended_output = 0;

static void sighandler(int signum);
static unsigned int xmlsocket_read_callback(const char *msg, size_t length, void *ud);
static void print_msg(h9msg_t* msg, int extend_output);

static void help(void) {
    h9_log_stderr("usage: h9spy [-dehvV] [-c host] [-p port]");
    h9_log_stderr("");
    h9_log_stderr("Options:");
    h9_log_stderr("   -c <host>  --coonnect=<host>");
    h9_log_stderr("   -d,        --debug");
    h9_log_stderr("   -e,        --extended");
    h9_log_stderr("   -h,        --help");
    h9_log_stderr("   -p <port>  --port=<port>");
    h9_log_stderr("   -v,        --verbose");
    h9_log_stderr("   -V,        --Version");
    exit(EXIT_FAILURE);
}

static void usage(void) {
    h9_log_stderr("usage: h9spy [-dehvV] [-c host] [-p port]");
    exit(EXIT_FAILURE);
}

static void version(void) {
    h9_log_stderr("h9spy version %s by SQ8KFH", H9D_VERSION);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int verbose = H9_LOG_STDERR;
    int debug = 0;

    int c;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"connect",     required_argument, 0, 'c' },
                {"debug",       no_argument,       0, 'd' },
                {"extended",    no_argument,       0, 'e' },
                {"help",        no_argument,       0, 'h' },
                {"port",        required_argument, 0, 'p' },
                {"verbose",     no_argument,       0, 'v' },
                {"version",     no_argument,       0, 'V' },
                {0,             0,                 0,  0  }
        };

        c = getopt_long(argc, argv, "c:dehp:vV", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            /*case 0:
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                usage();
                break;*/
            case 'c':
                //cfgfile = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            case 'e':
                extended_output = 1;
                break;
            case 'h':
                help();
                break;
            case 'p':
                //pidfile = optarg;
                break;
            case 'v':
                if (!verbose) verbose = H9_LOG_WARN;
                ++verbose;
                break;
            case 'V':
                version();
                break;
            default:
                usage();
                break;
        }
    }

    h9_log_init(verbose, debug, 1);

    h9_xmlsocket_t *xmlsocket = h9_xmlsocket_connect("127.0.0.1", "7878", 100);
    if (! xmlsocket) {
        h9_log_stderr("Connection error");
        return EXIT_FAILURE;
    }

    size_t length;
    char *msg = h9_xmlmsg_build_h9subscribe(&length, "msg", 1);

    h9_xmlsocket_send(xmlsocket, msg, length);

    signal(SIGINT, sighandler);

    while (run) {
        if (h9_xmlsocket_recv(xmlsocket, xmlsocket_read_callback, NULL) < 0) {
            h9_log_stderr("Disconnected");
            h9_xmlsocket_free(xmlsocket);
            return EXIT_FAILURE;
        }
    }

    h9_xmlsocket_free(xmlsocket);
    return EXIT_SUCCESS;
}

static void sighandler(int signum) {
    run = 0;
    exit(EXIT_SUCCESS);
}

static unsigned int xmlsocket_read_callback(const char *msg, size_t length, void *ud) {
    void *res;
    int ret = h9_xmlmsg_parse(msg, length, &res, 1);

    if (ret == H9_XMLMSG_MSG && res) {
        h9msg_t *msg = (h9msg_t*)res;
        print_msg(msg, extended_output);
        h9msg_free(msg);
    }
    else {
        h9_log_stderr("unknown message: %d", ret);
    }

    return 1;
}

static void print_msg(h9msg_t* msg, int extend_output) {
    printf("%-3hu -> %-3hu priority: %c; type: %2hhu; dlc: %hhu; endpoint '%s'; data: ",
           msg->source_id, msg->destination_id,
           msg->priority == H9MSG_PRIORITY_HIGH ? 'H' : 'L',
           msg->type, msg->dlc,
           msg->endpoint);
    for (int i = 0; i < msg->dlc; ++i) {
        printf("%02hhX", msg->data[i]);
    }
    printf("\n");
    if (extend_output) {
        printf("           type: %s\n", h9msg_type_name(msg->type));
    }
}
