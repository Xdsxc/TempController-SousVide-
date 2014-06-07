#ifndef PWM_H
#define PWM_H 1
#include <stdint.h>

void pwm_start(void);
void pwm_stop(void);
void pwm_change_settings(uint8_t duty_cycle, uint16_t period_ms);
#endif
