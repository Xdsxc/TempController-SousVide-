#include <avr/io.h>
#include <avr/sleep.h>
#include "timer.h"
#include "scheduler.h"
#include "state_machines.h"
#define NUM_TASKS  3
const uint8_t TASK_GCD = 200;
const uint8_t DISPCTRL_PERIOD = 200;
const uint16_t TEMPCTRL_PERIOD = 200;
const uint8_t MENUCTRL_PERIOD = 200;

struct Task tasks[NUM_TASKS];

void timer_isr()
{
  for (uint8_t i = 0; i < NUM_TASKS; i++) {
    if (tasks[i].elapsed_time == tasks[i].period) {
      tasks[i].state = tasks[i].tickFx(tasks[i].state);
      tasks[i].elapsed_time = 0;
    }
    tasks[i].elapsed_time += 1;
  }
}

int main(void)
{
  initialize_task(&tasks[0], MENUCTRL_PERIOD/TASK_GCD, &SM_MenuController);
  initialize_task(&tasks[1], TEMPCTRL_PERIOD/TASK_GCD, &SM_TemperatureController);
  initialize_task(&tasks[2], DISPCTRL_PERIOD/TASK_GCD, &SM_DisplayController);
  timer_set(TASK_GCD);
  timer_on();
  set_sleep_mode(0);
  while (1) {
    sleep_mode();
  }
}
