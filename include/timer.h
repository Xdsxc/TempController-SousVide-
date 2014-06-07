#ifndef AVR_TIMER_H_
#define AVR_TIMER_H_

void timer_on();
void timer_off();
void timer_set(uint16_t thousandths);
void timer_isr();
#endif
