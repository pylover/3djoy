#include "common.h"
#include "gcode.h"

#define STEP "1"


int gcodeget(struct js_event *e, char *outbuff, int *outlen) {
    if (e->type == JS_EVENT_AXIS) {
        if (e->number == JS_AXIS_X) {
            if (e->value > 0) {
                *outlen = sprintf(outbuff, "G0X" STEP);
                return GCODE_REPEAT;
            }
            else if (e->value < 0) {
                *outlen = sprintf(outbuff, "G0X-" STEP);
                return GCODE_REPEAT;
            }
            else {
                return OK;
            }
        }
    }
    return ERR;;
}


