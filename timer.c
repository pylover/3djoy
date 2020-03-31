#include "timer.h"
#include "common.h"

#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>


static int timerfd;
static int timerstate = TIMER_OFF;


int timersetup(int epollfd) {
    struct epoll_event ev;

    timerfd = timerfd_create(CLOCK_REALTIME, 0);
    if (timerfd == ERR) {
        perror("Cannot create timerfd");
        return ERR;
    }

    ev.events = EPOLLIN;
    ev.data.fd = timerfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timerfd, &ev) == ERR) {
        perrorf("epoll_ctl: ADD, input device");
        return ERR;
    }

    return timerfd;
}


void timerset(int s) {
    int err;
    struct itimerspec new_value;
    struct timespec now;

    if (timerstate == s) {
        return;
    }
   
    if (s) {
        clock_gettime(CLOCK_REALTIME, &now);
        new_value.it_value.tv_sec = now.tv_sec + ((now.tv_nsec + TIMER_DELAY_NS) / TIMER_NS);
        new_value.it_value.tv_nsec = (now.tv_nsec + TIMER_DELAY_NS) % TIMER_NS;
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

int timerread() {
    unsigned long t;
    int err;
    if (!timerstate) {
        return OK;
    }
    err = read(timerfd, &t, sizeof(unsigned long));
    if (err != sizeof(unsigned long)) {
        return ERR;
    }
    return OK;
}
