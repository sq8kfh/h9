#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "h9_log.h"
#include "h9d_cfg.h"
#include "h9d_select_event.h"
#include "h9d_server.h"
#include "h9d_client.h"
#include "h9d_endpoint.h"
#include "h9d_trigger.h"
#include "h9d_metricses.h"

#define DEFAULT_LOG_LEVEL H9_LOG_WARN

static void help(void) {
    h9_log_stderr("usage: h9d [-dDhvV] [-c config_file] [-p pid_file]");
    h9_log_stderr("");
    h9_log_stderr("Options:");
    h9_log_stderr("   -c <cfg_file> --config=<cfg_file>");
    h9_log_stderr("   -d,           --debug");
    h9_log_stderr("   -D,           --nodaemonize");
    h9_log_stderr("   -h,           --help");
    h9_log_stderr("   -p <pid_file> --pidfile=<pid_file>");
    h9_log_stderr("   -v,           --verbose");
    h9_log_stderr("   -V,           --Version");
    exit(EXIT_FAILURE);
}

static void usage(void) {
    h9_log_stderr("usage: h9d [-dDhvV] [-c config_file] [-p pid_file]");
    exit(EXIT_FAILURE);
}

static void version(void) {
    h9_log_stderr("h9d version %s by SQ8KFH", H9D_VERSION);
    exit(EXIT_FAILURE);
}

static void daemonize(void) {
    if (h9d_cfg_getbool("daemonize") == h9d_cfg_true) {
    #ifdef __APPLE__
        pid_t pid, sid;

        /* already a daemon */
        if (getppid() == 1) return;

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then we can exit the parent process. */
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }

        /* At this point we are executing as the child process */

        /* Change the file mode mask */
        umask(0);

        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
            exit(EXIT_FAILURE);
        }

        /* Change the current working directory.  This prevents the current
           directory from being locked; hence not being able to remove it. */
        if ((chdir("/")) < 0) {
            exit(EXIT_FAILURE);
        }

        /* Redirect standard files to /dev/null */
        freopen("/dev/null", "r", stdin);
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
    #else
        daemon(1, 1);
    #endif
    }
}

static void savepid(void) {
    char *tmp_pidfile = h9d_cfg_getstr("pid_file");

    if (tmp_pidfile != NULL) {
        h9_log_debug("saving pid file %s", tmp_pidfile);
        FILE *pid_file = fopen(tmp_pidfile, "a+");
        if (!pid_file) {
            perror("open pid file");
            exit(EXIT_FAILURE);
        }
        int rc = flock(fileno(pid_file), LOCK_EX | LOCK_NB);
        if (rc != 0) {
            if (errno == EWOULDBLOCK) {
                h9_log_stderr("h9d is already running\n");
            } else {
                perror("save pid file");
            }
            exit(EXIT_FAILURE);
        } else {
            ftruncate(fileno(pid_file), 0);
            fprintf(pid_file, "%d\n", getpid());
        }
        fflush(pid_file);
    }
}

void sighandler(int signum) {
    h9_log_err("caught signal %d", signum);
    h9d_select_event_stop();
}

int main(int argc, char **argv) {
    int verbose = DEFAULT_LOG_LEVEL;
    int nodaemonize = 0;
    int debug = 0;
    char *pidfile = NULL;
    char *cfgfile = H9D_CONFIG_FILE;

    int c;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"config",      required_argument, 0, 'c' },
                {"debug",       no_argument,       0, 'd' },
                {"nodaemonize", no_argument,       0, 'D' },
                {"help",        no_argument,       0, 'h' },
                {"pidfile",     required_argument, 0, 'p' },
                {"verbose",     no_argument,       0, 'v' },
                {"version",     no_argument,       0, 'V' },
                {0,             0,                 0,  0  }
        };

        c = getopt_long(argc, argv, "c:dDhp:vV", long_options, &option_index);
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
                cfgfile = optarg;
                break;
            case 'd':
                debug = 1;
                break;
            case 'D':
                nodaemonize = 1;
                break;
            case 'h':
                help();
                break;
            case 'p':
                pidfile = optarg;
                break;
            case 'v':
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

    h9d_cfg_init(cfgfile);
    h9_log_init(verbose, debug, 0);

    if (nodaemonize == 1)
        h9d_cfg_setbool("daemonize", h9d_cfg_false);

    if (verbose == DEFAULT_LOG_LEVEL)
        verbose = h9d_cfg_getint("verbose");
    else
        h9d_cfg_setint("verbose", verbose + 1);

    if (pidfile)
        h9d_cfg_setstr("pid_file", pidfile);

    h9_log_set_verbose(verbose);

    daemonize();
    h9_log_notice("h9d started");
    savepid();

    signal(SIGINT, sighandler);

    h9d_trigger_init();
    h9d_metrices_init();
    h9d_select_event_init();
    //h9d_endpoint_init();

    for (const char *endpoint_name = h9d_cfg_endpoint(); endpoint_name; endpoint_name = h9d_cfg_endpoint()) {
        h9d_endpoint_t *endpoint = h9d_endpoint_addnew(h9d_cfg_endpoint_getstr("connect"),
                                                       endpoint_name,
                                                       (size_t)h9d_cfg_endpoint_getint("recv_buf_size"),
                                                       (unsigned int)h9d_cfg_endpoint_getint("throttle_level"),
                                                       h9d_cfg_endpoint_getbool("nonblock"),
                                                       h9d_cfg_endpoint_getbool("auto_respawn"));
        if (!endpoint) {
            h9d_select_event_free();
            h9_log_err("cannot open endpoint");
            h9_log_crit("h9d terminated abnormally");
            return EXIT_FAILURE;
        }
        h9d_select_event_add(endpoint->ep_imp->fd, H9D_SELECT_EVENT_READ | H9D_SELECT_EVENT_DISCONNECT,
                             (h9d_select_event_func_t *) h9d_endpoint_process_events, endpoint);
    }

    h9d_server_t *sm = h9d_server_init(7878);
    h9d_select_event_add(sm->socket_d, H9D_SELECT_EVENT_READ | H9D_SELECT_EVENT_DISCONNECT,
                         (h9d_select_event_func_t*)h9d_server_process_events, sm);

    h9d_client_init((size_t)h9d_cfg_getint("client_recv_buffer_size"),
                    h9d_cfg_getbool("xmlmsg_schema_validation") == h9d_cfg_true ? 1 : 0);

    if (h9d_select_event_loop(h9d_cfg_getint("time_trigger_period")) == 0) {
        //h9d_endpoint_free();
        //h9d_endpoint_free();
        h9d_trigger_free();

        h9_log_crit("h9d terminated");
        return EXIT_SUCCESS;
    }

    /*struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    printf("%d %d\n", rusage.ru_utime.tv_usec, rusage.ru_stime.tv_usec);*/

    h9_log_crit("h9d terminated abnormally");
    return EXIT_FAILURE;
}
