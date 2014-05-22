#ifndef PWM_H
#define PWM_H 1
#include <stdint.h>

void pwm_start(uint8_t duty_cycle, uint16_t period_ms);
void pwm_pause(void);
void pwm_resume(void);

#endif
