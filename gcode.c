#include "common.h"
#include "gcode.h"

int gcodeget(struct js_event *e, char *outbuff) {
    
    if (e->type == JS_EVENT_AXIS) {
        if (e->number == JS_AXIS_X) {
            
        }
    }
    return OK;
}


