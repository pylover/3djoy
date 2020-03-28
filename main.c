
#include "common.h"
#include "tty.h"
#include "cli.h"
#include "js.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>


//static int serialfd;


static int _process_inputevent(struct epoll_event *e) {
    //char buff[8];
	//int bytes, fd;
    //fd = e->data.fd;
	//if (e->events & EPOLLIN) {
    //    bytes = read(fd, buff, 8);
    //    if (bytes == ERR) {
    //        perrorf("Cannot read from serial interface");
    //        return bytes;
    //    }
    //    else if (bytes == 0) {
    //        perrorf("Serial file closed");
    //        return ERR;
    //    }
    //    // TODO: write to stdout, perhaps!
    //    //connection_broadcast(buff, bytes);
	//}
	return OK;
}



int main(int argc, char **argv) {
    int inputfd, fdcount, err, i;
    struct epoll_event events[MAXEVENTS], *e;
    
    // Parse command line arguments
    cliparse(argc, argv);

    // Create epoll instance
    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        perrorf("Cannot create epoll file descriptor");
        exit(EXIT_FAILURE);
    }
    
    // TODO: Open inputfd
    inputfd = jsopen();
    if (inputfd == ERR) {
        perrorf("Cannot open input device: %s", settings.device);
        exit(EXIT_FAILURE);
    }

    //// Open and register serial port
    //serialfd = serialopen();
    //if (serialfd == -1) {
    //    perrorf("Cannot open serial device: %s", settings.device);
    //    exit(EXIT_FAILURE);
    //}

    /* Main Loop */
    while (1) {
        fdcount = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (fdcount == -1) {
            perrorf("epoll_wait returned: %d", fdcount);
            exit(EXIT_FAILURE);
        }
        
        for (i = 0; i < fdcount; i++) {
            e = &events[i];
            if (e->data.fd == inputfd) {
                err = _process_inputevent(e);
                if (err == ERR) {
                    exit(EXIT_FAILURE);
                }
            } 
        }
    }
    return EXIT_SUCCESS;
}
