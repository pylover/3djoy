#include "common.h"
#include "output.h"
#include "gcode.h"


// TODO: Move all to common.h
#define STEP        1
#define FEEDRATE    1000
#define Z_FEEDRATE  500
#define E_FEEDRATE  200


static unsigned char nuzzle = 0;


int gcodeget(struct js_event *e, char *outbuff, int *outlen) {
    char axis;
    int step;

    if (e->value == 0) {
        return GCODE_STOPREPEATE;
    }

    if (e->type == JS_EVENT_AXIS) {
        axis = e->number == JS_AXIS_X? 'X': 'Y';
        
        /* value -1/1 */
        step = e->value > 0? -STEP: STEP;
        if (axis == 'Y') {
            step = -step;
        }
        *outlen = sprintf(outbuff, "G1F%d%c%d", FEEDRATE, axis, step);
        return GCODE_REPEAT;
    }
    else if (e->type == JS_EVENT_BUTTON) {
        switch (e->number) {
            case JS_NINE:   // Home
                *outlen = sprintf(outbuff, "G28");
                return OK;

            case JS_ONE:    // Z+
            case JS_THREE:  // Z-
                step = e->number == JS_THREE? -STEP: STEP;
                *outlen = sprintf(outbuff, "G1F%dZ%d", Z_FEEDRATE, step);
                return GCODE_REPEAT;

            case JS_L1:     // Z + .1
            case JS_L2:     // Z - .1
                *outlen = sprintf(outbuff, "G1F20Z%c0.1", JS_L2? '-': '');
                return OK;;
 
            case JS_FOUR:    // E+
            case JS_TWO:     // E-
                step = e->number == JS_TWO? -STEP: STEP;
                *outlen = sprintf(outbuff, "G1F%dE%d", E_FEEDRATE, step);
                return GCODE_REPEAT;

            case JS_R1:     // Nuzzle +
            case JS_R2:     // Nuzzle -
                step = e->number == JS_R2? -STEP: STEP;
                nuzzle += step;
                if (nuzzle == 255) {
                    nuzzle -= step;
                    return GCODE_STOPREPEATE;
                }
                *outlen = sprintf(outbuff, "M104S%d", nuzzle);
                return GCODE_REPEAT;
        }
    }
    else if ((e->type & JS_EVENT_INIT) == JS_EVENT_INIT) {
        infoln(
            "Input init command: %d, %d, %d", 
            e->type, e->number, e->value);
        return GCODE_IGNORE;
    }
    return ERR;;
}


int gcodeinit(int fd) {
    int err;

    err = output(fd, "G91\n");    
    if (err == ERR) {
        return ERR;
    }

    return OK;
}

