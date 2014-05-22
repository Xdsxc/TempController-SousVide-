#include "pid.h"

void pid_initialize(struct PID *pid, int16_t setpoint, uint16_t sample_time_ms, int16_t max_output, int16_t min_output)
{
  pid->sample_time_ms = sample_time_ms;
  pid->_iterm = 0;
  pid_tune(pid, 0, 0, 0);
  pid->set_point = setpoint;
  pid->last_reading = 0;
  pid->max_output = max_output;
  pid->min_output = min_output;
}

int16_t pid_compute(struct PID *pid, uint8_t sensor_reading)
{
  int16_t error = pid->set_point - sensor_reading;  
  int16_t delta_error;
  int16_t output;
  pid->_iterm += pid->_ki*error;
  if (pid->_iterm > pid->max_output) {
    pid->_iterm = pid->max_output;
  } else if (pid->_iterm < pid->min_output) {
    pid->_iterm = pid->min_output;
  }

  delta_error = sensor_reading - pid->last_reading;
  pid->last_reading = sensor_reading;

  output = pid->_kp*error + pid->_iterm - (pid->_kd*delta_error);
  if (output > pid->max_output) {
    output = pid->max_output;
  } else if (output < pid->min_output) {
    output = pid->min_output;
  }
  return output;
}

void pid_tune(struct PID *pid, float kp, float ki, float kd)
{
  pid->_kp = kp;
  pid->_ki = ki*(float)(pid->sample_time_ms/1000);
  pid->_kd = kd*(float)(pid->sample_time_ms/1000);
}

void pid_change_setpoint(struct PID *pid, uint16_t set_point)
{
  pid->set_point = set_point;
  pid->_iterm = 0;
}
