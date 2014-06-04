#ifndef PID_H
#define PID_H 1
#include <stdint.h>
struct PID
{
  int16_t set_point;
  int16_t last_reading;
  int16_t max_output;
  int16_t min_output;
  uint16_t sample_time_ms;
  float _kp, _ki, _kd;
  float _iterm;
};
void pid_initialize(struct PID *pid, int16_t setpoint, uint16_t sample_time_ms, int16_t max_output, int16_t min_output);
int16_t pid_compute(struct PID *pid, uint8_t sensor_reading);
void pid_tune(struct PID *pid, float kp, float ki, float kd);
void pid_change_setpoint(struct PID *pid, uint16_t set_point);
#endif
