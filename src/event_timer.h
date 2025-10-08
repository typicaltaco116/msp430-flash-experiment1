#pragma once
#include <msp430.h>
/*****************************************************************
* FILENAME: event_timer.h
* DESCRIPTION: Intended to record the length of very fast events
* Timer will overflow after only 65.536 mS by default.
* The slow timer SLOW_EVENT_TIMER_START uses ACLK (~32KHz) as its
*   input and thus will overflow after exactly 2 seconds.
* Even slower timers can be implemented using software counting
*   in the future.
* RESOURCE USAGE: Timer A0, no interrupts atm
******************************************************************/

// multipling these macros by the timers value will convert to seconds
#define EVENT_TIMER_SEC_FLT 1 / (1.024E6) 
#define SLOW_EVENT_TIMER_SEC_FLT 0.00003051757
#define EVENT_TIMER_USEC_FLT 1 / 1.024
#define SLOW_EVENT_TIMER_MSEC_FLT 0.03051757812

#define EVENT_TIMER_START {\
  TA0CTL |= TACLR;\
  TA0CTL = TASSEL_2 + ID__1 + MC_2;\
}
#define SLOW_EVENT_TIMER_START {\
  TA0CTL |= TACLR;\
  TA0CTL = TASSEL_1 + ID__1 + MC_2;\
}
#define EVENT_TIMER_STOP {\
  _event_timer_value = TA0R - 3;\
  TA0CTL &= ~MC_3;\
}

extern unsigned int _event_timer_value;

void event_timer_start(void);

void event_timer_stop(void);

