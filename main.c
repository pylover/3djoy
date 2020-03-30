
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


static int outfd = -1;



static int _process_inputevent(int fd) {
    struct js_event jse;
    char gcode[256];
    int bytes;

    bytes = read(fd, &jse, sizeof(jse));
    if (bytes < sizeof(jse)) {
        perrorf("Read input device error");
        return ERR;
    }
    
    bytes = gcodeget(&jse, gcode);
    if (bytes == ERR) {
        perrorf(
            "Unrecognized command: %d, %d, %d", 
            jse.type, jse.number, jse.value
        );
        return ERR;
    }
    gcode[bytes] = 0;
    printfln("%s, %d, %d, %d", gcode, jse.type, jse.number, jse.value);
	return OK;
}



int main(int argc, char **argv) {
    int inputfd, err;
    
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

    /* Main Loop */
    while (1) {
        err = _process_inputevent(inputfd);
        if (err == ERR) {
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
