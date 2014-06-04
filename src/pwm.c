#include <avr/io.h>
#include "pwm.h"
void pwm_change_settings(uint8_t duty_cycle, uint16_t period_ms)
{
  // PWM, phase and frequency correct
  // TOP = ICR1
  // Clear OC1A at OCR1A match
  DDRD |= _BV(PD5);
  ICR1 = period_ms*125;
  OCR1A = ICR1*((float)duty_cycle/100);
  TCCR1A = _BV(COM1A1);
  TCCR1B |=  _BV(WGM13);
}

void pwm_stop(void)
{
  TCCR1B &= ~(_BV(CS11) | _BV(CS10));
}

void pwm_start(void)
{
  TCCR1B |= _BV(CS11) | _BV(CS10);
}
