
#include "common.h"
#include "cli.h"
#include "js.h"
#include "tty.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>


static int _process_inputevent(int fd) {
    struct js_event jse;
    int bytes = read(fd, &jse, sizeof(jse));
    if (bytes < sizeof(jse)) {
        perrorf("Read input device error");
        return ERR;
    }
    printfln("%d, %d, %d, %d", jse.time, jse.value, jse.type, jse.number);
    
	return OK;
}



int main(int argc, char **argv) {
    int inputfd, serialfd, err;
    
    // Parse command line arguments
    cliparse(argc, argv);

    inputfd = open(settings.device, O_RDONLY);
    if (inputfd == ERR) {
        perrorf("Cannot open input device: %s", settings.device);
        exit(EXIT_FAILURE);
    }
   
    // Open and register serial port
    serialfd = serialopen();
    if (serialfd == -1) {
        perrorf("Cannot open serial device: %s", settings.device);
        exit(EXIT_FAILURE);
    }

    /* Main Loop */
    while (1) {
        err = _process_inputevent(inputfd);
        if (err == ERR) {
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
