#include "config.h"
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <csignal>

#include "h9_log.h"
#include "h9_xmlsocket.h"
#include "h9_xmlmsg.h"

static int run = 1;

static void sighandler(int signum);
static unsigned int xmlsocket_read_callback(const char *msg, size_t length, void *ud);
static void process_msg(h9_xmlmsg_t *msg);

static void help(void) {
    h9_log_stderr("usage: h9stats [-dhvV] [-c host] [-p port]");
    h9_log_stderr("");
    h9_log_stderr("Options:");
    h9_log_stderr("   -c <host>  --coonnect=<host>");
    h9_log_stderr("   -d,        --debug");
    h9_log_stderr("   -h,        --help");
    h9_log_stderr("   -p <port>  --port=<port>");
    h9_log_stderr("   -v,        --verbose");
    h9_log_stderr("   -V,        --Version");
    exit(EXIT_FAILURE);
}

static void usage(void) {
    h9_log_stderr("usage: h9stats [-dehvV] [-c host] [-p port]");
    exit(EXIT_FAILURE);
}

static void version(void) {
    h9_log_stderr("h9stats version %s by SQ8KFH", H9_VERSION);
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
                {"help",        no_argument,       0, 'h' },
                {"port",        required_argument, 0, 'p' },
                {"verbose",     no_argument,       0, 'v' },
                {"version",     no_argument,       0, 'V' },
                {0,             0,                 0,  0  }
        };

        c = getopt_long(argc, argv, "c:dhp:vV", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'c':
                //cfgfile = optarg;
                break;
            case 'd':
                debug = 1;
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
    char *msg = h9_xmlmsg_build_h9subscribe(&length, "metrics", 1);

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

static unsigned int xmlsocket_read_callback(const char *xmlmsg, size_t length, void *ud) {
    h9_xmlmsg_t *tmp_xmlmsg = h9_xmlmsg_parse(xmlmsg, length, 1);
    if (!tmp_xmlmsg) {
        return 1;
    }

    if (tmp_xmlmsg->type == H9_XMLMSG_METRICS) {
        process_msg(tmp_xmlmsg);
    }
    else {
        h9_log_stderr("unknown message: %d", tmp_xmlmsg->type);
    }
    h9_xmlmsg_free(tmp_xmlmsg);

    return 1;
}

static void process_msg(h9_xmlmsg_t *msg) {
    h9_xmlmsg_metrics_t *metrics = h9_xmlmsg_get_metrics_list(msg);
    for (h9_xmlmsg_metrics_t *p = metrics; p->name; ++p) {
        printf("%s: %s\n", p->name, p->value);
    }
    h9_xmlmsg_get_metrics_list_free(metrics);
}
