#ifndef H_TIMER
#define H_TIMER

#define TIMER_NS            1000000000
#define TIMER_INTERVAL_NS   50000000  
#define TIMER_DELAY_NS      000000000  
#define TIMER_ON  1
#define TIMER_OFF 0


int timersetup();
void timerset(int s); 
int timerread();

#endif
