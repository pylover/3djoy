
#include "common.h"
#include "cli.h"
#include "js.h"
#include "gcode.h"
#include "timer.h"
#include "output.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


static int outfd = -1;
static int epollfd;
static char gcode[256];


// TODO: remove all stdout logs


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
    
    bytes = dprintf(outfd, "%s\n", gcode);
	return OK;
}


int main(int argc, char **argv) {
    int inputfd, err, fdcount, i;
    struct epoll_event ev, events[EPOLL_MAXEVENTS];
    
    // Parse command line arguments
    cliparse(argc, argv);
    
    // epoll
    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perrorf("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }

    outfd = openoutput(epollfd);// stdout;
    if (outfd == ERR) {
        perrorf("Cannot open ouput: %s", settings.output);
        exit(EXIT_FAILURE);
    }
    
    timerfd = timersetup(epollfd);
    if (timerfd == ERR) {
        perrorf("Cannot create timer");
        exit(EXIT_FAILURE);
    }

    // input
    inputfd = open(settings.input, O_RDONLY);
    if (inputfd == ERR) {
        perrorf("Cannot open input device: %s", settings.input);
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = inputfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, inputfd, &ev) == ERR) {
        perrorf("epoll_ctl: EPOLL_CTL_ADD, input device");
        exit(EXIT_FAILURE);
    }
   
    
    // Setup output device
    gcodeinit(outfd);

    /* Main Loop */
    unsigned long c, t;
    char buff[1025];
    while (1) {
        fdcount = epoll_wait(epollfd, events, EPOLL_MAXEVENTS, -1);
        if (fdcount == -1) {
            perrorf("epoll_wait returned: %d", fdcount);
            exit(EXIT_FAILURE);
        }
    
        // TODO: read from ouput device to make buffers empty!
        for (i = 0; i < fdcount; i++) {
            ev = events[i];
            if (ev.data.fd == inputfd) {
                err = _process_inputevent(inputfd);
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            }
            if (ev.data.fd == outfd) {
                err =  read(outfd, buff, 1024);
                if (err == ERR) {
                    perrorf("cannot read from out device");
                }
                buff[err] = 0;
                printf("%s", buff);
            }
            // Repeat
            else if (timerstate && (ev.data.fd == timerfd)) {
                err = read(timerfd, &t, sizeof(unsigned long));
                if (err != sizeof(unsigned long)) {
                    perrorf("Cannot read from timer");
                }
                c += t;
                dprintf(outfd, "%s\n", gcode);
            }
        }
    }
    return EXIT_SUCCESS;
}
