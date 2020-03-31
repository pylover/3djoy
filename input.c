#include "common.h"

#include <fcntl.h>
#include <sys/epoll.h>


static int inputfd;


int inputopen(int epollfd) {
    struct epoll_event ev;
    inputfd = open(settings.input, O_RDONLY);
    if (inputfd == ERR) {
        return ERR;
    }

    ev.events = EPOLLIN;
    ev.data.fd = inputfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, inputfd, &ev) == ERR) {
        perrorf("epoll_ctl: EPOLL_CTL_ADD, input device");
        return ERR;
    }
    return inputfd;
}   

