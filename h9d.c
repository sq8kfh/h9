#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "h9_log.h"
#include "h9d_cfg.h"
#include "h9d_select_event.h"
#include "h9d_server_module.h"

static void help(void) {
    h9_log_stderr("usage: h9d [-DhvV] [-c config_file] [-p pid_file]\n");
    h9_log_stderr("\n");
    h9_log_stderr("Options:\n");
    h9_log_stderr("   -c <cfg_file> --config=<cfg_file>\n");
    h9_log_stderr("   -D,           --nodaemonize\n");
    h9_log_stderr("   -h,           --help\n");
    h9_log_stderr("   -p <pid_file> --pidfile=<pid_file>\n");
    h9_log_stderr("   -v,           --verbose\n");
    h9_log_stderr("   -V,           --Version\n");
    exit(EXIT_FAILURE);
}

static void usage(void) {
    h9_log_stderr("usage: h9d [-DhvV] [-c config_file] [-p pid_file]\n");
    exit(EXIT_FAILURE);
}

static void version(void) {
    h9_log_stderr("h9d version %s by SQ8KFH\n", H9D_VERSION);
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

int tmp_func(void * ud, int event_type, int timer) {
    printf("Data is available now.\n");
    char buf[100];
    read(1, buf, 100);
    return 0;
}

int main(int argc, char **argv) {
    int verbose = 0;
    int nodaemonize = 0;
    char *pidfile = NULL;
    char *cfgfile = H9D_CONFIG_FILE;

    int c;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                {"config",      required_argument, 0, 'c' },
                {"nodaemonize", no_argument,       0, 'D' },
                {"help",        no_argument,       0, 'h' },
                {"pidfile",     required_argument, 0, 'p' },
                {"verbose",     no_argument,       0, 'v' },
                {"version",     no_argument,       0, 'V' },
                {0,             0,                 0,  0  }
        };

        c = getopt_long(argc, argv, "c:Dhp:vV", long_options, &option_index);
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
    h9_log_init();

    if (nodaemonize == 1)
        h9d_cfg_setbool("daemonize", h9d_cfg_false);

    if (verbose > 0)
        h9d_cfg_setint("verbose", verbose + 1);

    if (pidfile)
        h9d_cfg_setstr("pid_file", pidfile);

    daemonize();
    savepid();

    h9d_select_event_init();

    h9d_server_module_t *sm = h9d_server_module_init(7878);
    h9d_select_event_add(sm->socket_d, H9D_SELECT_EVENT_READ, (h9d_select_event_func_t*)h9d_server_module_process_events, sm);

    h9d_select_event_add(0, H9D_SELECT_EVENT_READ, (h9d_select_event_func_t*)tmp_func, NULL);

    h9d_select_event_loop();

    sleep(20);
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    printf("%d %d\n", rusage.ru_utime.tv_usec, rusage.ru_stime.tv_usec);

    return EXIT_SUCCESS;
}
