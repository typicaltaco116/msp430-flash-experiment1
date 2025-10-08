#include "event_timer.h"

unsigned int _event_timer_value = 0;

void event_timer_start(void)
{
  TA0CTL |= TACLR;
  TA0CTL |= TASSEL_2 + ID__1 + MC_2;
  // SMCLK = 1.048MHz, no divider, continous mode
}

void event_timer_stop(void)
{
  _event_timer_value = TA0R - 7; // function call 4 CC + mov 3 CC
  TA0CTL &= ~MC_3; // halt timer
}

