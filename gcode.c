#include "common.h"
#include "gcode.h"

#define STEP 1


int gcodeget(struct js_event *e, char *outbuff, int *outlen) {
    char axis;
    int step;
    if (e->type == JS_EVENT_AXIS) {
        if (e->value == 0) {
            return OK; // stop repeate
        }

        axis = e->number == JS_AXIS_X? 'X': 'Y';
        step = e->value > 0? -STEP: STEP;
        if (axis == 'Y') {
            step = -step;
        }
        *outlen = sprintf(outbuff, "G0%c%d", axis, step);
        return GCODE_REPEAT;
    }
    return ERR;;
}


