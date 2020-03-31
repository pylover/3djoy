
#include "common.h"
#include "cli.h"
#include "js.h"
#include "gcode.h"
#include "timer.h"
#include "output.h"
#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


static char gcode[256];


static int _process_inputevent(int fd, int outfd) {
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
   
    output(outfd, "%s\n", gcode);
	return OK;
}


int main(int argc, char **argv) {
    int epollfd, inputfd, err, fdcount, timerfd, outfd, i;
    struct epoll_event ev, events[EPOLL_MAXEVENTS];
    
    // Parse command line arguments
    cliparse(argc, argv);
    
    // epoll
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perrorf("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }

    // output
    outfd = outputopen(epollfd);
    if (outfd == ERR) {
        perrorf("Cannot open ouput: %s", settings.output);
        exit(EXIT_FAILURE);
    }
   
    // timer
    timerfd = timersetup(epollfd);
    if (timerfd == ERR) {
        perrorf("Cannot create timer");
        exit(EXIT_FAILURE);
    }

    // input
    inputfd = inputopen(epollfd);
    if (inputfd == ERR) {
        perrorf("Cannot open input device: %s", settings.input);
        exit(EXIT_FAILURE);
    }

    // Setup output device
    gcodeinit(outfd);
    // TODO: error handling

    /* Main Loop */
    while (1) {
        fdcount = epoll_wait(epollfd, events, EPOLL_MAXEVENTS, -1);
        if (fdcount == -1) {
            perrorf("epoll_wait returned: %d", fdcount);
            exit(EXIT_FAILURE);
        }
    
        for (i = 0; i < fdcount; i++) {
            ev = events[i];
            if (ev.data.fd == inputfd) {
                err = _process_inputevent(inputfd, outfd);
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            }
            if (ev.data.fd == outfd) {
                err = outputread();
                if (err == ERR) {
                    perrorf("cannot read from out device");
                    exit(EXIT_FAILURE);
                }
            }
            else if (ev.data.fd == timerfd) {
                // Repeat
                err = timerread();
                if (err == ERR) {
                    perrorf("Cannot read from timer");
                    exit(EXIT_FAILURE);
                }
                output(outfd, "%s\n", gcode);
            }
        }
    }
    return EXIT_SUCCESS;
}
