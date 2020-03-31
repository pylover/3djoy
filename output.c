#include "common.h"
#include "tty.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>


static int outfd;
/*
 * stdout
 * /dev/tty*
 * example.com:5600
 * /run/serialhub.socket
 */

int outputopen(int epollfd) {
    struct epoll_event ev;
    
    if (settings.output[0] == '-') {
        infoln("Using stdout as output device");
        outfd = STDOUT;
    }
    else if (strnstr(settings.output, "/dev/tty", 8) != NULL) {
        perrorf("Using tty device: %s as output.", settings.output);
        outfd = serialopen();
        // Wait some times to allow marlin to initialize the serial communication
        sleep(2);
    }
    else {
        perrorf("Invalid output device: %s", settings.output);
        return ERR;
    }

    ev.events = EPOLLIN;
    ev.data.fd = outfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, outfd, &ev) == ERR) {
        perrorf("epoll_ctl: ADD, output device");
        return ERR;
    }
    
    return outfd;
}


int outputread() {
    int err;
    char buff[1025];
    err = read(outfd, buff, 1024);
    if (err == ERR) {
        return err;
    }
    buff[err] = 0;
    info("%s", buff);
    return OK;
}
