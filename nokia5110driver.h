#ifndef NOKIA5110DRIVER_H_
#define NOKIA5110DRIVER_H_
#include <stdint.h>
#include "bitmap2d.h"
struct Bitmap2D;
struct Nokia5110LCD
{
  struct Bitmap2D screen_buf;
  volatile uint8_t *control_port;
  uint8_t _dc;
  uint8_t _cs;
  uint8_t _rst;
};

/* N5110 Commands.
  -- EXTENDED AND BASIC MODE COMMANDS --
  BASIC: Set basic command mode
  EXTENDED: Set extended command mode
  POWERDOWN: Display standby
  VERTICAL_ADDRESSING: Set vertical addressing. Send "BASIC" to reset addressing. 
                       To keep vertical addressing between commands, make sure to bitwise
                       OR any of the top row commands with this one before sending a command.
  -- BASIC MODE ONLY --
  BLANK: Set display blank
  ALL_ON: Turn on entire display
  NORMAL: Normal display mode
  INVERSE: Invert the display
  SET_X: Set the X address of RAM: 0 <= X <= 83
  SET_Y: Set the Y address of RAM: 0 <= Y <= 5 
  -- EXTENDED MODE ONLY --
  BIAS: Set display bias. Bitwise OR the command with a value from 0 to 7. 
  CONTRAST: Set display contrast. Bitwise OR the command with a 7-bit value.
*/
enum {N5110_BASIC = 0x20, N5110_EXTENDED = 0x21, N5110_POWERDOWN = 0x24, N5110_VERTICAL_ADDRESSING = 0x22,
              N5110_SET_X = 0x80, N5110_SET_Y = 0x40, N5110_BIAS = 0x10, N5110_BLANK = 0x08, 
              N5110_ALL_ON = 0x09, N5110_NORMAL = 0x0C, N5110_INVERSE = 0x0D, N5110_CONTRAST = 0x80};

void n5110_initialize(struct Nokia5110LCD *lcd, volatile uint8_t *SPI_port, volatile uint8_t *control_port,
    uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ss, uint8_t  d_c, uint8_t cs,
      uint8_t rst);

void n5110_reset(struct Nokia5110LCD *lcd);
void n5110_send_command(const struct Nokia5110LCD *lcd, uint8_t cmd);
void n5110_send_data(const struct Nokia5110LCD *lcd, uint8_t cmd);
void n5110_clear(struct Nokia5110LCD *lcd);
void n5110_display_screen(const struct Nokia5110LCD *lcd);
void n5110_set_screen(struct Nokia5110LCD *lcd, uint8_t *buf);
#endif
