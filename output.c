#include "common.h"
#include "tty.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

/*
 * stdout
 * /dev/tty*
 * example.com:5600
 * /run/serialhub.socket
 */

int openoutput(int epollfd) {
    int fd;
    struct epoll_event ev;
    
    if (settings.output[0] == '-') {
        perrorf("Using stdout as output device");
        fd = STDOUT;
    }
    else if (strnstr(settings.output, "/dev/tty", 8) != NULL) {
        perrorf("Using tty device: %s as output.", settings.output);
        fd = serialopen();
        // Wait some times to allow marlin to initialize the serial communication
        sleep(2);
    }
    else {
        perrorf("Invalid output device: %s", settings.output);
        return ERR;
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == ERR) {
        perrorf("epoll_ctl: ADD, output device");
        return ERR;
    }
    
    return fd;
}
