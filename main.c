
#include "common.h"
#include "cli.h"
#include "js.h"
#include "tty.h"
#include "gcode.h"
#include "timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


static int outfd = -1;
static int epollfd;
static char gcode[256];



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
        timerset(TIMER_OFF);
        return OK;
    }
    else if (err == GCODE_REPEAT) {
        timerset(TIMER_ON);
    }
    else if (err == GCODE_STOPREPEATE) {
        timerset(TIMER_OFF);
        return OK;
    }
    
    printf("%s\n", gcode);
    fflush(stdout);
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

    timerfd = timersetup(epollfd);
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
    
    // Setup output device
    printfln("G91");

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
