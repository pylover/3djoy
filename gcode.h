#ifndef H_GCODE
#define H_GCODE

#include "js.h"

#define GCODE_REPEAT        1
#define GCODE_STOPREPEATE   2
#define GCODE_IGNORE        3

int gcodeget(struct js_event *e, char *out, int *outlen);
int gcodeinit(int fd);
#endif
