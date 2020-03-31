#include "common.h"
#include "output.h"
#include "gcode.h"

#define STEP 1
#define FEEDRATE    1000


int gcodeget(struct js_event *e, char *outbuff, int *outlen) {
    char axis;
    int step;
    if (e->type == JS_EVENT_AXIS) {
        if (e->value == 0) {
            return GCODE_STOPREPEATE;
        }

        axis = e->number == JS_AXIS_X? 'X': 'Y';
        step = e->value > 0? -STEP: STEP;
        if (axis == 'Y') {
            step = -step;
        }
        *outlen = sprintf(outbuff, "G1F%d%c%d", FEEDRATE, axis, step);
        return GCODE_REPEAT;
    }
    else if (e->type == JS_EVENT_BUTTON) {
        switch (e->number) {
            case JS_NINE:
            
                if (e->value == 1) {
                    *outlen = sprintf(outbuff, "G28");
                    return OK;
                }
        }
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

