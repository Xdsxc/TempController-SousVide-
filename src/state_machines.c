#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pid.h"
#include "keypad.h"
#include "pwm.h"
#include "bitmap2d.h"
#include "nokia5110driver.h"
#include "onewire.h"
#include "onewire_ds18b20.h"
#include "images/text_table.h"
#include "images/splash_screen.h"

/* Signals SM_DisplayController to update the display */
/* Owned by SM_MenuController, used by SM_DisplayController */
static bool DO_WRITE_DISPLAY = false;

/* Signals SM_TemperatureController to start output */
/* Owned by SM_MenuController, used by SM_TemperatureController */
static bool DO_RUN = false;

/* The screen used by SM_DisplayController */
/* Owned by SM_MenuController, used by SM_DisplayController */
static const struct Bitmap2D *MAIN_DISPLAY;

/* Owned by SM_TemperatureController, used by SM_MenuController */
static float LAST_TEMP = 0;
static uint8_t PID_OUTPUT = 0;

/* Owned by SM_MenuController, used by SM_TemperatureController */
static uint8_t TEMP_SET_POINT = 0;

/* Calculates offsets for each char in string str relative to the char bitmap
 * loaded in images/text_table.h. Then superimposes each letter into the bitmap. */
static void overlay_string(struct Bitmap2D *dest, const char *str, uint8_t x, uint8_t y)
{
  static bool initialized = false;
  static struct Bitmap2D text;
  if (!initialized) {
    bitmap_initialize(&text);
    bitmap_set_buffer(&text, text_table, 750*8, 8, false);
    initialized = true;
  }
  uint8_t len = strlen(str);
  for (uint8_t i = 0; i < len; i++) {
    bitmap_superimpose(&text, (str[i] - ' ')*10, 0, 6,
                          dest, (i + x)*6, y);
  }
}

/* Controls writing to the LCD. The written data is determined by what
 * MAIN_SCREEN points to */
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

/* Controls reading the temperature and calculating the approrpiate output
 * based on the reading. Must have at least 200ms period to account for the
 * time required for a temperature conversion (based on the sensor datasheet and measurements). */
int8_t SM_TemperatureController(int8_t state)
{
  //enum {NotConnected, Idle, InitializeOutputs, Run};
  enum {NotConnected, Idle, InitializeOutputs, StartConversion, ReadTemp};
  static struct OnewireDevice therm;
  static struct PID pid;
  static bool is_connected = false;
  static float last_reading;
  switch (state) 
  {
    case NotConnected:
      state = is_connected ? Idle : NotConnected;
      break;
    case Idle:
      state = DO_RUN ? InitializeOutputs : Idle;
      break;
    case InitializeOutputs:
      state = is_connected ? StartConversion : NotConnected;
      break;
    case StartConversion:
      state = is_connected && DO_RUN? ReadTemp : NotConnected;
      break;
    case ReadTemp:
      if (!is_connected) {
        state = NotConnected;
      } else if (last_reading != DS18B20_CONVERSION_IN_PROGRESS && DO_RUN) {
        state = StartConversion;
      } else if (last_reading != DS18B20_CONVERSION_IN_PROGRESS && !DO_RUN){
        state = Idle;
      }
      break;
    default:
      state = NotConnected;
      break;
  }
  switch (state) 
  {
    case NotConnected:
      is_connected = ds18b20_setup_device(&therm, &PORTA, 0);
      pid_initialize(&pid, 0, TEMP_SET_POINT, 100, 0);
      pwm_change_settings(0, 10);
      last_reading = 0;
      if (is_connected && therm.resolution != LOW) {
        ds18b20_set_resolution(&therm, LOW);
      }
      break;
    case Idle:
      break;
    case InitializeOutputs:
      pwm_start();
      pid_initialize(&pid, TEMP_SET_POINT, 200, 100, 0);
      pid_tune(&pid, 10, 0.7, 1.8);
      break;
    case StartConversion:
      is_connected = ds18b20_start_conversion(&therm);
      break;
    case ReadTemp:
      {
        last_reading = ds18b20_read_temperature(&therm);
        if (last_reading == ONEWIRE_DEVICE_DISCONNECTED) {
          is_connected = false;
          LAST_TEMP = 0;
        } else if (last_reading != DS18B20_CONVERSION_IN_PROGRESS) {
          LAST_TEMP = last_reading;
          PID_OUTPUT = pid_compute(&pid, LAST_TEMP);
          pwm_change_settings(PID_OUTPUT, 10);
        }
      }
      break;
    default:
      break;
  }
  return state;
}

/* Controls the main user interface by writing the screen to a buffer and
 * points MAIN_DISPLAY to that buffer. Handlers user I/O. */
int8_t SM_MenuController(int8_t state)
{
  enum {Init, MainMenu, DispSetTemp, SetTemp, DispSetTime, SetTime, Run, RunMenu};
  static struct Bitmap2D canvas;
  static int16_t i;
  static uint8_t hours;
  static uint8_t minutes;
  static char buf_[10] = "";
  switch (state) 
  {
    case Init:
      state = MainMenu;
      break;
    case MainMenu:
      /* Check keypad until user enters '1' */
      buf_[0]  = read_keypad();
      if (buf_[0] == '1') {
        state = DispSetTemp;
      }
      break;
    case DispSetTemp:
      /* Write intial temperature set screen */
      state = SetTemp;
      i = 100;
      break;
    case SetTemp:
      /* Get the temperature from the user via keypad. Update
       * screen to display user input. 'i' starts at 100, representing
       * the left-most decimal digit of 3, and is decremented by a factor
       * of 10 for each iteration. So iteration 1 will start at 100, get the
       * digit, add it to the set point, then decrement to 10. 10 decrements to
       * 1, then 1 decrements to -1 which signals the end of input.  */
      if (i < 0) {
        state = DispSetTime;
      }
      break;
    case DispSetTime:
      /* Write initial time set screen */
      state = SetTime;
      i = 0;
      break;
    case SetTime:
      /* Get the time from the user via keyboard. Three digits must be
       * received, the hour, and the minutes. The format is input from
       * left to write in H:MM format. Each time a digit
       * is received, 'i' is incremented until it is greater than 3. At that
       * point the input is complete and the system begins output. */
      if (i > 2) {
        i = 300;
        state = Run;
      }
      break;
    case Run:
      /* In this state, 'i' is used as a counter to translate between the SM's
       * period and the real-time clock time. Since this SM is being ran at
       * 200ms period, that corresponds to 5 ticks per second, which is 300
       * ticks per minute */
      if (minutes == 0 && hours == 0) {
        DO_RUN = false;
        state = Init;
      } else if ((buf_[0] = read_keypad()) != '\0') {
        state = RunMenu;
      }
      break;
    case RunMenu:
      buf_[1] = read_keypad();
      if (buf_[1] != buf_[0]) {
        buf_[0] = buf_[1];
        if (buf_[1] == '1') {
          state = Init;
        } else if (buf_[1] == '2') {
          state = Run;
        }
      }
      break;
    default:
      state = Init;
      break;
  }

  switch (state) 
  {
    case Init:
      DDRC = 0xF0;
      PORTC = 0x0F;
      hours = 0;
      minutes = 0;
      TEMP_SET_POINT = 0;
      bitmap_initialize(&canvas);
      bitmap_set_buffer(&canvas, splash_screen, N5110_WIDTH, N5110_HEIGHT, true);
      MAIN_DISPLAY = &canvas;
      DO_WRITE_DISPLAY = true;
      break;
    case MainMenu:
      memset(buf_, 0, sizeof(buf_));
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Main Menu", 2, 0);
      overlay_string(&canvas, "Press (1)", 3, 16);
      overlay_string(&canvas, "to begin.", 3, 24);
      MAIN_DISPLAY = &canvas;
      DO_WRITE_DISPLAY = true;
      break;
    case DispSetTemp:
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Set", 0, 0);
      overlay_string(&canvas, "Temperature", 2, 8);
      overlay_string(&canvas, "_", 6, 24);
      MAIN_DISPLAY = &canvas;
      DO_WRITE_DISPLAY = true; 
      break;
    case SetTemp: 
      {
        uint8_t keypad_value;
        /* Check if the user is still holding down the same button. This SM
         * requires the user to relase the key between inputs. This allows the
         * SM to remain in this state instead of explicily creating a new state
         * for the release of the key. Since read_keypad will return a '\0'
         * when no input is sent, the user will be allowed to entire the same
         * digit twice in a row */
        buf_[1] = read_keypad();
        if (buf_[1] != buf_[0]) {
          buf_[0] = buf_[1];
          buf_[1] = '\0';
          keypad_value = keypad_char_to_hex(buf_[0]);
          if (keypad_value < 0x0A) {
            switch (i)
            {
              case 100:
                overlay_string(&canvas, strcat(buf_, "_"), 6, 24);
                TEMP_SET_POINT += keypad_value*100;
                i = 10;
                break;
              case 10:
                overlay_string(&canvas, strcat(buf_, "_"), 7, 24);
                TEMP_SET_POINT += keypad_value*10;
                i = 0;
                break;
              case 0:
                overlay_string(&canvas, buf_, 8, 24);
                TEMP_SET_POINT += keypad_value;
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
        uint8_t keypad_value;
        buf_[1] = read_keypad();
        if (buf_[1] != buf_[0]) {
          buf_[0] = buf_[1];
          buf_[1] = '\0';
          keypad_value = keypad_char_to_hex(buf_[0]);
          if (keypad_value < 0x0A) {
            switch (i) {
              case 2:
                minutes += keypad_value;
                overlay_string(&canvas, buf_, 9, 16);
                i = 3;
                break;
              case 1:
                if (keypad_value < 6) {
                  minutes += 10*keypad_value;
                  overlay_string(&canvas, strcat(buf_, "_"), 8, 16);
                  i = 2;
                }
                break;
              case 0:
                hours += keypad_value;
                overlay_string(&canvas, strcat(buf_, ":_"), 6, 16);
                i = 1;
                break;
              default:
                break;
            }
            DO_WRITE_DISPLAY = true;
          }
        }
      }
      break;
    case Run:
     DO_RUN = true;
     /* Check if it is time to decrement the clock */
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
        i = 300;
      } 

      /* Update the main screen every second */
      if (i % 5 == 0) {
        bitmap_set_all(&canvas);
        overlay_string(&canvas, "Running", 3, 0);
        memset(buf_, 0, sizeof(buf_));
        /* Write time */
        overlay_string(&canvas, strcat(itoa(hours, buf_, 10), ":"), 4, 8);
        if (minutes < 10) {
          overlay_string(&canvas, "0", 6, 8);
          overlay_string(&canvas, itoa(minutes, buf_, 10), 7, 8);
        } else {
          overlay_string(&canvas, itoa(minutes, buf_, 10), 6, 8);
        }
        /* Write set temperature */
        overlay_string(&canvas, "S:", 0, 16);
        overlay_string(&canvas, itoa(TEMP_SET_POINT, buf_, 10), 2, 16);

        /* Write current temperature */
        overlay_string(&canvas, "C:", 7, 16);
        overlay_string(&canvas, dtostrf(LAST_TEMP, 5, 1, buf_), 9, 16);

        /* Write percentage output (duty cycle) */
        overlay_string(&canvas, strcat(itoa(PID_OUTPUT, buf_, 10), "%"), 4, 32);
        DO_WRITE_DISPLAY = true;
      }
      break;
    case RunMenu:
      bitmap_set_all(&canvas);
      overlay_string(&canvas, "Paused", 4, 0);
      overlay_string(&canvas, "1 -- Exit", 0, 16);
      overlay_string(&canvas, "2 -- Resume", 0, 24);
      DO_WRITE_DISPLAY = true;
      DO_RUN = false;
      break;
    default:
      break;
  }
  return state;
}

