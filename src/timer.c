/*
 * File: timer.c
 * Name: Dustin Guerrero (dguer006@ucr.edu)
 * Section: 021
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */ 
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
extern void timer_isr();

ISR(TIMER3_COMPA_vect)
{
  timer_isr();
}

void timer_on()
{
  // 64 Prescaler, CTC Mode
  TCCR3B |= _BV(CS31) | _BV(CS30) | _BV(WGM32);
}

void timer_off()
{
  // Clock disabled
  TCCR3B = 0x00;
}

void timer_set(uint16_t thousandths)
{
  // Set Timer/Counter1 compare match A interrupt
  TIMSK3 = _BV(OCIE3A);

  // Initialize counter to 0
  TCNT3 = 0;

  /* With FCPU = 8000000, and 64 prescaler, we get a new frequency 
   * of 125,000 ticks/s. We want 0.001 ticks/s, so 31250*0.001 = 125. */
  OCR3A = thousandths*125; 

  // Enable global interrupts
  SREG |= 0x80;
}
