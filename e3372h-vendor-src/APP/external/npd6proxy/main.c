/*
 *   This software is Copyright 2011 by Sean Groarke <sgroarke@gmail.com>
 *   All rights reserved.
 *
 *   This file is part of npd6.
 *
 *   npd6 is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   npd6 is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with npd6.  If not, see <http://www.gnu.org/licenses/>.
 */

/* $Id: main.c 84 2012-02-29 09:46:20Z npd6Project $
 * $HeadURL: https://npd6.googlecode.com/svn/trunk/main.c $
 */

#include "includes.h"
#include "npd6.h"

#define NDP6PROXY_PIDFILE "/var/run/npd6proxy.pid"

unsigned char   msgdata[MAX_MSG_SIZE *2];
char usage_str[] =
{
"\n"
"  -c, --config=PATH      Sets the config file.  Default is /etc/npd6.conf.\n"
"  -d, --debug            Sets the normal debug level.\n"
"  -D, --debug2           Sets full debug level. (Lots!)\n"
"  -b, --background       Run in background, otherwise daemonize.\n"
"  -h, --help             Show this help screen.\n"
"  -l  --logfile=PATH     Sets the log file, else default to syslog. If \"-l -\" then stdout/stderr used.\n"
"  -v, --version          Print the version and quit.\n"
};

struct option prog_opt[] =
{
    {"config",  required_argument, 0, 'c'},
    {"debug",   optional_argument, 0, 'd'},
    {"debug2",  optional_argument, 0, 'D'},
    {"help",    optional_argument, 0, 'h'},
    {"logfile", required_argument, 0, 'l'},
    {"version", optional_argument, 0, 'v'},
    {"background", optional_argument, 0, 'b'},
    {0, 0, 0, 0}
};

#define OPTIONS_STR "c:dDhl:vf"

struct Interface *IfaceList = NULL;

int main(int argc, char *argv[])
{
    char logfile[FILENAME_MAX] = "";
    char switchifname[IFNAMSIZ] = {0};
    int c, err;
    int pid = 0;
    FILE *pidfp = NULL;

    // Default some globals
    strncpy(configfile, NPD6_CONF, FILENAME_MAX);
    strncpy( interfacestr, NULLSTR, sizeof(NULLSTR));
    memset( prefixaddrstr, 0, sizeof(prefixaddrstr));
    interfaceIdx = -1;
    daemonize = 0;
    // Default black/whitelisting to OFF
    listType = NOLIST;
    // Default config file values as required
    naLinkOptFlag = 0;
    nsIgnoreLocal = 1;
    naRouter = 1;
    debug = 0;
    maxHops = MAXMAXHOPS;

    /* Parse the args */
    while ((c = getopt_long(argc, argv, OPTIONS_STR, prog_opt, NULL)) > 0)
    {
        if (c==-1)
            break;

        switch (c) {
        case 'c':
            strcpy(configfile, optarg);
            break;
        case 'l':
            strcpy(logfile, optarg);
            break;
        case 'd':
            debug=1;
            break;
        case 'D':
            debug=2;
            break;
        case 'b':
            daemonize=1;
            break;
        case 'v':
            showVersion();
            return 0;
            break;
        case 'h':
            showUsage();
            return 0;
            break;
        }
    }
    
	/* Seems like about the right time to daemonize (or not) */
    if (daemonize)
    {
        if (daemon(0, 0) < 0 )
        {
            flog(LOG_ERR, "Failed to daemonize. Error: %s", strerror(errno) );
            exit(1);
        }
    }
	
	pid = getpid();
	if ((pidfp = fopen(NDP6PROXY_PIDFILE, "w")) != NULL) {
		fprintf(pidfp, "%d\n", pid);
		fclose(pidfp);
	}
    else
    {
        printf("**************** Open Pid file faild *********************** \r\n");
    }
	
    while (0 >= strlen(prefixaddrstr))
    {
        addr6match(NULL, NULL, 0);
        sleep(1);
    }
    /* Sort out where to log */
    if ( strlen(logfile) )
    {
        if (strlen(logfile) == 1 && (logfile[0]='-')  )
            logging = USE_STD;
        else
            logging = USE_FILE;
    }
    else
    {
        logging = USE_SYSLOG;
    }

    /* Open the log and config*/
    if ( (logging == USE_FILE) && (openLog(logfile) < 0)  )
    {
        printf("Exiting. Error in setting up logging correctly.\n");
        exit (1);
    }
    flog(LOG_INFO, "*********************** npd6 *****************************");

    if ( readConfig(configfile) )
    {
        flog(LOG_ERR, "Error in config file: %s", configfile);
        return 1;
    }

    flog(LOG_INFO, "Using normalised prefix %s/%d", prefixaddrstr, prefixaddrlen);
    flog(LOG_DEBUG2, "ifIndex for %s is: %d", interfacestr, interfaceIdx);

    err = init_sockets();
    if (err) {
        flog(LOG_ERR, "init_sockets: failed to initialise one or both sockets.");
        exit(1);
    }

    /* Set allmulti on the interface */
    while (0 > npd6getwan(switchifname))
    {
        sleep(1);
    }
    if_allmulti(switchifname, TRUE);
    if_allmulti(interfacestr, TRUE);
    /* Set up signal handlers */
    signal(SIGUSR1, usersignal);
    signal(SIGUSR2, usersignal);
    signal(SIGHUP, usersignal);
    signal(SIGINT, usersignal);
    signal(SIGTERM, usersignal);    // Typically used by init.d scripts

    /* And off we go... */
    dispatcher();

    flog(LOG_ERR, "Fell back out of dispatcher... This is impossible.");
    return 0;
}

void dispatcher(void)
{
    struct pollfd   fds[2];
    unsigned int    msglen;
    int             rc, err;

    memset(fds, 0, sizeof(fds));
    fds[0].fd = sockpkt;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = -1;
    fds[1].events = 0;
    fds[1].revents = 0;

    for (;;)
    {
        flog(LOG_WARNING, "poll continue");
        rc = poll(fds, sizeof(fds)/sizeof(fds[0]), DISPATCH_TIMEOUT);

        if (rc > 0)
        {
            if (   fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)
                || fds[1].revents & (POLLERR | POLLHUP | POLLNVAL) )
            {
                flog(LOG_WARNING, "Major socket error on fds[0 or 1].fd");
                // Try and recover
                close(sockpkt);
                close(sockicmp);
                // Allow a moment for things to maybe return to normal...
                sleep(1);
                err = init_sockets();
                if (err)
                {
                    flog(LOG_ERR, "init_sockets: failed to reinitialise one or both sockets.");
                    exit(1);
                }
                memset(fds, 0, sizeof(fds));
                fds[0].fd = sockpkt;
                fds[0].events = POLLIN;
                fds[0].revents = 0;
                fds[1].fd = -1;
                fds[1].events = 0;
                fds[1].revents = 0;
                continue;
            }
            else if (fds[0].revents & POLLIN)
            {
                msglen = get_rx(msgdata, sockpkt, 0);
                // msglen is checked for sanity already within get_rx()
                flog(LOG_DEBUG2, "get_rx() gave msg with len = %d", msglen);

                // Have processNS() do the rest of validation and work...
                if (0 < msglen)
                {
                	flog(LOG_DEBUG2, "processNS begin");
                	processNS(msgdata, msglen);
                	flog(LOG_DEBUG2, "processNS end");
                }
                continue;
            }
            else if ( rc == 0 )
            {
                flog(LOG_DEBUG, "Timer event");
                // Timer fired?
                // One day. If we implement timers.
            }
            else if ( rc == -1 )
            {
                flog(LOG_ERR, "Weird poll error: %s", strerror(errno));
                continue;
            }

            flog(LOG_DEBUG, "Timed out of poll(). Timeout was %d ms", DISPATCH_TIMEOUT);
        }
    }
}


//*******************************************************
// Give advice.
void showUsage(void)
{
    fprintf(stderr, "usage: %s %s\n", pname, usage_str);
}
