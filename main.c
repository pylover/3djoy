
#include "common.h"
#include "cli.h"
#include "js.h"
#include "tty.h"
#include "gcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>


static int outfd = -1;
static int epollfd;
static int timerfd;
static char gcode[256];


#define TIMER_INTERVAL_NS   100000000  
#define ON  1
#define OFF 0


static int timersetup() {
    int fd;
    struct epoll_event ev;

    fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == ERR) {
        perror("Cannot create timerfd");
        return ERR;
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == ERR) {
        perrorf("epoll_ctl: ADD, input device");
        return ERR;
    }

    return fd;
}

static int timerstate = OFF;

static void timerset(int s) {
    int err;
    struct itimerspec new_value;
    struct timespec now;

    if (timerstate == s) {
        return;
    }
   
    if (s) {
        clock_gettime(CLOCK_REALTIME, &now);
        new_value.it_value.tv_sec = now.tv_sec;
        new_value.it_value.tv_nsec = now.tv_nsec;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_nsec = TIMER_INTERVAL_NS;
    }
    else {
        memset(&new_value, 0, sizeof(new_value));
    }

    err = timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &new_value, NULL);
    if (err == ERR) {
        perror("Cannot set timerfd");
        exit(EXIT_FAILURE);
    }

    timerstate = s;
}


static int _process_inputevent(int fd) {
    struct js_event jse;
    int bytes, err;

    bytes = read(fd, &jse, sizeof(jse));
    if (bytes < sizeof(jse)) {
        perrorf("Read input device error");
        return ERR;
    }
    
    err = gcodeget(&jse, gcode, &bytes);
    if (err == ERR) {
        perrorf(
            "Unrecognized command: %d, %d, %d", 
            jse.type, jse.number, jse.value
        );
    }
    else if (err == GCODE_REPEAT) {
        timerset(ON);
    }
    else {
        timerset(OFF);
    }
    
    //gcode[bytes] = 0;
    printfln("%s, %d, %d, %d, repeat: %d", gcode, jse.type, jse.number, jse.value, err);
	return OK;
}



int main(int argc, char **argv) {
    int inputfd, err, fdcount, i;
    struct epoll_event ev, events[EPOLL_MAXEVENTS];
    
    // Parse command line arguments
    cliparse(argc, argv);

    inputfd = open(settings.input, O_RDONLY);
    if (inputfd == ERR) {
        perrorf("Cannot open input device: %s", settings.input);
        exit(EXIT_FAILURE);
    }

    outfd = 0; // stdout;
    // TODO: Open the output device
    //serialfd = serialopen();
    //if (serialfd == -1) {
    //    perrorf("Cannot open serial device: %s", settings.device);
    //    exit(EXIT_FAILURE);
    //}
    

    // epoll
    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perrorf("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }

    timerfd = timersetup();
    if (timerfd == ERR) {
        perrorf("Cannot create timer");
        exit(EXIT_FAILURE);
    }

    // input
    ev.events = EPOLLIN;
    ev.data.fd = inputfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, inputfd, &ev) == ERR) {
        perrorf("epoll_ctl: EPOLL_CTL_ADD, input device");
        exit(EXIT_FAILURE);
    }

    /* Main Loop */
    unsigned long c, t;
    while (1) {
        fdcount = epoll_wait(epollfd, events, EPOLL_MAXEVENTS, -1);
        if (fdcount == -1) {
            perrorf("epoll_wait returned: %d", fdcount);
            exit(EXIT_FAILURE);
        }
        
        for (i = 0; i < fdcount; i++) {
            ev = events[i];
            if (ev.data.fd == inputfd) {
                err = _process_inputevent(inputfd);
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            }
            else if (timerstate && (ev.data.fd == timerfd)) {
                err = read(timerfd, &t, sizeof(unsigned long));
                if (err != sizeof(unsigned long)) {
                    perrorf("Cannot read from timer");
                }
                c += t;
                printfln("%s", gcode);
            }
        }
    }
    return EXIT_SUCCESS;
}
