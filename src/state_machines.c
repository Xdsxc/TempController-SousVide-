#include <stdint.h>
#include <ctype.h>
#include <stdlib.h>
#include <avr/io.h>
#include <string.h>
#include <stdbool.h>
#include "pid.h"
#include "keypad.h"
#include "pwm.h"
#include "bitmap2d.h"
#include "nokia5110driver.h"
#include "ow.h"
#include "ow_ds18b20.h"
#include "images/letters.h"
#include "images/test_screen.h"
static bool DO_WRITE_DISPLAY = false;
static bool DO_RUN = false;
static const struct Bitmap2D *MAIN_DISPLAY;
static float SM_TEMPERATURESENSOR_READING = 0;
static uint8_t TEMP_SET_POINT = 0;
static uint8_t PID_OUTPUT = 0;
static void overlay_string(struct Bitmap2D *dest, const char *str, uint8_t x, uint8_t y)
{
  static bool initialized = false;
  static struct Bitmap2D text;
  if (!initialized) {
    bitmap_initialize(&text);
    bitmap_set_buffer(&text, letters, 750*8, 8, false);
    initialized = true;
  }
  uint8_t len = strlen(str);
  for (uint8_t i = 0; i < len; i++) {
    bitmap_superimpose(&text, (str[i] - ' ')*10, 0, 6,
                          dest, (i + x)*6, y);
  }
}

/* 100ms Period for 10 frames per second */
int8_t SM_DisplayController(int8_t state)
{
  enum {Init, Idle, WriteDisplay};
  static struct Nokia5110LCD lcd;
  switch (state) 
  {
    case Init:
      state = Idle;
      break;
    case Idle:
      if (DO_WRITE_DISPLAY) {
        state = WriteDisplay;
      } 
      break;
    case WriteDisplay:
      state = Idle;
      break;
    default:
      state = Init;
      break;
  }
  switch (state) 
  {
    case Init:
      n5110_initialize(&lcd, &PORTB, &PORTB, 5, 6, 7, 4, 0, 1, 2);
      n5110_send_command(&lcd, N5110_INVERSE);
      break;
    case Idle:
      break;
    case WriteDisplay:
      n5110_display_screen(&lcd, MAIN_DISPLAY);	
      DO_WRITE_DISPLAY = false;
      break;
    default:
      break;
  }
  return state;
}

/* Reccomend ~190ms period due to conversion time for 10-bit resolution */
int8_t SM_TemperatureSensor(int8_t state)
{
  enum {NotConnected, StartConversion, ReadTemperature};
  static struct OnewireDevice therm;
  static struct PID pid;
  static bool is_connected = false;
  static uint8_t last_set; 
  switch (state) {
    case NotConnected:
      //if (is_connected) {
        state = StartConversion;
      //}
      break;
    case StartConversion:
      /*
      if (!is_connected) {
        state = NotConnected;
      } else {
        state = ReadTemperature;
      }*/
      break;
    case ReadTemperature:
      /*
      if (!is_connected) {
        state = NotConnected;
      } else if (SM_TEMPERATURESENSOR_READING != DS18B20_CONVERSION_IN_PROGRESS) {
        state = StartConversion;
      }*/
      SM_TEMPERATURESENSOR_READING = ds18b20_get_temperature(&therm);
        break;
        default:
        state = NotConnected;
        break;
      }
      switch (state) 
      {
        case NotConnected:
          ds18b20_setup_device(&therm, &PORTA, 0);
          pid_initialize(&pid, 0, TEMP_SET_POINT, 100, 0);
          last_set = TEMP_SET_POINT;
          pwm_change_settings(0, 250);
          if (therm.resolution != LOW) {
            ds18b20_set_resolution(&therm, LOW);
          }
          break;
        case StartConversion:
          if (DO_RUN) {
            pwm_start();
            pid_initialize(&pid, TEMP_SET_POINT, 250, 100, 0);
            pid_tune(&pid, 10, 0.7, 1.8);
          }
          SM_TEMPERATURESENSOR_READING = ds18b20_get_temperature(&therm);
          PID_OUTPUT = pid_compute(&pid, SM_TEMPERATURESENSOR_READING);
          pwm_change_settings(PID_OUTPUT, 10);
          break;
        case ReadTemperature:
          SM_TEMPERATURESENSOR_READING = ds18b20_get_temperature(&therm);
          /*
          if (SM_TEMPERATURESENSOR_READING == ONEWIRE_DEVICE_DISCONNECTED) {
            is_connected = false;*
          } */
          break;
        default:
          break;
      }
  return state;
}

int8_t SM_MenuController(int8_t state)
{
  enum {Init, MainMenu, DispSetTemp, SetTemp, DispSetTime, SetTime, Run};
  static struct Bitmap2D splash_screen;
  static struct Bitmap2D main_menu;
  static struct Bitmap2D canvas;
  static int16_t i = 10;
  static char buf_[10] = "";
  static uint8_t hours;
  static uint8_t minutes;
  switch (state) 
  {
    case Init:
      state = MainMenu;
      break;
    case MainMenu:
      switch (buf_[0]) 
      {
        case '1':
          state = DispSetTemp;
          break;
        default:
          break;
      }
      break;
    case DispSetTemp:
      state = SetTemp;
      i = 100;
      break;
    case SetTemp:
      if (i < 0) {
        state = DispSetTime;
      }
      break;
    case DispSetTime:
      state = SetTime;
      i = 0;
      break;
    case SetTime:
      if (i > 3) {
        i = 600;
        state = Run;
      }
      break;
    case Run:
      if (minutes == 0 && hours == 0) {
        DO_RUN = false;
        state = MainMenu;
      }
      break;
    default:
      state = Init;
      break;
  }
  switch (state) {
    case Init:
      DDRC = 0xF0;
      PORTC = 0x0F;
      bitmap_initialize(&splash_screen);
      bitmap_initialize(&canvas);
      bitmap_resize(&canvas, N5110_WIDTH, N5110_HEIGHT);
      bitmap_set_buffer(&splash_screen, test_screen, N5110_WIDTH, N5110_HEIGHT, false);
      MAIN_DISPLAY = &splash_screen;
      DO_WRITE_DISPLAY = true;
      break;
    case MainMenu:
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Main Menu", 2, 0);
      overlay_string(&canvas, "Press (1)", 3, 16);
      overlay_string(&canvas, "to begin.", 3, 24);
      MAIN_DISPLAY = &canvas;
      DO_WRITE_DISPLAY = true;
      buf_[0] = read_keypad();
      break;
    case DispSetTemp:
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Set", 0, 0);
      overlay_string(&canvas, "Temperature", 2, 8);
      MAIN_DISPLAY = &canvas;
      DO_WRITE_DISPLAY = true; 
      break;
    case SetTemp: 
      {
        uint8_t reading_hex;
        buf_[1] = read_keypad();
        if (buf_[0] != buf_[1]) {
          buf_[0] = buf_[1];
          buf_[1] = '\0';
          reading_hex = keypad_char_to_hex(buf_[0]);
          if (reading_hex < 0x0A) {
            switch (i)
            {
              case 100:
                overlay_string(&canvas, buf_, 6, 18);
                TEMP_SET_POINT += reading_hex*100;
                i = 10;
                break;
              case 10:
                overlay_string(&canvas, buf_, 7, 18);
                TEMP_SET_POINT += reading_hex*10;
                i = 0;
                break;
              case 0:
                overlay_string(&canvas, buf_, 8, 18);
                TEMP_SET_POINT += reading_hex;
                i = -1;
                break;
            }
          }
          DO_WRITE_DISPLAY = true;
        }
      }
      break;
    case DispSetTime:
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Set Time", 3, 0);
      overlay_string(&canvas, "_:", 6, 16);
      DO_WRITE_DISPLAY = true;
      break;
    case SetTime:
      {
        uint8_t reading_hex;
        buf_[1] = read_keypad();
        if (buf_[0] != buf_[1]) {
          buf_[0] = buf_[1];
          buf_[1] = '\0';
          reading_hex = keypad_char_to_hex(buf_[0]);
          if (reading_hex < 0x0A) {
            switch (i) {
              case 3:
                minutes += reading_hex;
                break;
              case 2:
                break;
              case 1:
                i = 2;
                minutes += 10*reading_hex;
                break;
              case 0:
                hours += reading_hex;
                break;
              default:
                break;
            }
            overlay_string(&canvas, buf_, 6 + i++, 16);
            TEMP_SET_POINT += reading_hex*i;
            DO_WRITE_DISPLAY = true;
          }
        }
      }
      break;
    case Run:
     DO_RUN = true;
      if (--i <= 0) {
        if (minutes == 0) {
          if (hours == 0) {
            DO_RUN = false;
          } else {
            hours -= 1;
            minutes = 59;
          }
        } else {
          minutes -= 1;
        }
        i = 600;
      } 
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Running", 3, 0);
      memset(buf_, 0, sizeof(buf_));
      overlay_string(&canvas, strcat(itoa(hours, buf_, 10), ":"), 4, 8);
      if (minutes < 10) {
        overlay_string(&canvas, "0", 6, 8);
        overlay_string(&canvas, itoa(minutes, buf_, 10), 7, 8);
      } else {
        overlay_string(&canvas, itoa(minutes, buf_, 10), 6, 8);
      }
      //if (SuM_TEMPERATURESENSOR_READING != DS18B20_CONVERSION_IN_PROGRESS) {
        memset(buf_, 0, sizeof(buf_));
        overlay_string(&canvas, "T:", 0, 16);
        overlay_string(&canvas, itoa(TEMP_SET_POINT, buf_, 10), 2, 16);
        overlay_string(&canvas, "C:", 7, 16);
        overlay_string(&canvas, dtostrf(SM_TEMPERATURESENSOR_READING, 5, 1, buf_), 9, 16);
      //}
      memset(buf_, 0, sizeof(buf_));
      overlay_string(&canvas, strcat(itoa(PID_OUTPUT, buf_, 10), "%"), 4, 24);
      DO_WRITE_DISPLAY = true;
      break;
    default:
      break;
  }
  return state;
}

