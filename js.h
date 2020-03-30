#ifndef H_JS
#define H_JS

#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */

#define JS_AXIS_X               0x00
#define JS_AXIS_Y               0x01


struct js_event {
    unsigned int time;       /* event timestamp in milliseconds */
	short value;             /* value */
	unsigned char type;      /* event type */
	unsigned char number;    /* axis/button number */
};


#endif

