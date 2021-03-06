#ifndef H_JS
#define H_JS

#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */

#define JS_AXIS_X               0
#define JS_AXIS_Y               1

#define JS_ONE                  0
#define JS_TWO                  1
#define JS_THREE                2
#define JS_FOUR                 3
#define JS_L1                   4
#define JS_R1                   5
#define JS_L2                   6
#define JS_R2                   7
#define JS_NINE                 8
#define JS_TEN                  9


struct js_event {
    unsigned int time;       /* event timestamp in milliseconds */
	short value;             /* value */
	unsigned char type;      /* event type */
	unsigned char number;    /* axis/button number */
};


#endif

