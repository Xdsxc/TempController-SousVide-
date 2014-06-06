#include <avr/io.h>
#include <avr/sleep.h>
#include "pwm.h"
#include "pid.h"
#include <stdbool.h>
#include "timer.h"
#include "scheduler.h"
#include "state_machines.h"
#define NUM_TASKS  3
const uint8_t TASK_GCD = 50;
const uint8_t DISPCTRL_PERIOD = 250;
const uint8_t TEMPSENS_PERIOD = 100;
const uint8_t MENUCTRL_PERIOD = 250;
struct Task tasks[4];
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
  DDRD = 0xFF;
  initialize_task(&tasks[0], MENUCTRL_PERIOD/TASK_GCD, &SM_MenuController);
  initialize_task(&tasks[1], DISPCTRL_PERIOD/TASK_GCD, &SM_DisplayController);
  initialize_task(&tasks[2], TEMPSENS_PERIOD/TASK_GCD, &SM_TemperatureSensor);
  timer_set(TASK_GCD);
  timer_on();
  while (1) {
  }
}
/*
int main(void) 
{
  DDRC = 0xFF;
  PORTC = 0xFF;
  struct Nokia5110LCD lcd;

  struct PID controller;
  struct OnewireDevice therm;
  ds18b20_setup_device(&therm, &PORTA, 0);
  pid_initialize(&controller, 95, 1, 100, 0);
  pid_tune(&controller, 3, 0.5, 2);

  struct Bitmap2D canvas;
  struct Bitmap2D text;
  bitmap_initialize(&canvas);//, (uint8_t*)test_screen, N5110_WIDTH, N5110_HEIGHT);
  bitmap_set_buffer(&canvas, test_screen, N5110_WIDTH, N5110_HEIGHT, false);
  bitmap_initialize(&text);//, (uint8_t*)letters, 750*8, 8);
  bitmap_set_buffer(&text, letters, 750*8, 8, false);
  n5110_display_screen(&lcd, &canvas);
  
  pwm_change_settings(pid_compute(&controller, (uint8_t)ds18b20_get_temperature(&therm)), 1);
  pwm_start();
  double temp;
  char buf[20];
  char linetwo[] = "Output: ";
  uint8_t len;
  int16_t val;
  char x[] = "%";
  while (1) {
    bitmap_set_all(&canvas);
    memset(buf, 0, 20);
    temp = ds18b20_get_temperature(&therm);
    val = pid_compute(&controller, temp);
    pwm_change_settings(val, 7);
    overlay_string(&canvas, &text, buf, 0, 0);
    overlay_string(&canvas, &text, linetwo, 0, 8);
    overlay_string(&canvas, &text, strcat(itoa(val, buf, 10), x), strlen(linetwo) - 1, 8);
    buf[0] = read_keypad();
    if (buf[0] != '\0') {
      buf[1] = '\0';
      overlay_string(&canvas, &text, buf,  0, 16);
    }
    n5110_display_screen(&lcd, &canvas);
    _delay_ms(10);
  }
} */
