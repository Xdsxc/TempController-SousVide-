#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stdint.h>

uint32_t gcd(uint32_t a, uint32_t b)
{
  uint32_t c;
  while(1){
    c = a % b;
    if( c == 0 ) { return b; }
    a = b;
    b = c;
  }
  return 0;
}
struct Task 
{
  // Tasks should have members that include: state, period,
  //a measurement of elapsed time, and a function pointer.
  int8_t state; //Task's current state
  uint16_t period; //Task period
  uint16_t elapsed_time; //Time elapsed since last task tick
  int8_t (*tickFx)(int8_t); //Task tick function
};

inline void initialize_task(struct Task* x, uint16_t task_period, int8_t (*task_callback)(int8_t))
{
  x->period = task_period;
  x->elapsed_time = task_period;
  x->tickFx = task_callback;
  x->state = -1;
}
#endif 
