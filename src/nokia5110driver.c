#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>
#include "pinmanip.h"
#include "nokia5110driver.h"
#include "bitmap2d.h"
#define N5110_MAX_MBITS 4
#define N5110_DEFAULT_BIAS 0x4
#define N5110_DEFAULT_CONTRAST 45
static void spi_master_initialize(volatile uint8_t *spi_port, uint8_t mosi, 
    uint8_t miso, uint8_t sck, uint8_t ss);
static inline void spi_write(uint8_t data);

void n5110_initialize(struct Nokia5110LCD *lcd, volatile uint8_t *spi_port, volatile uint8_t *control_port,
    uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t ss, uint8_t  d_c, uint8_t cs, uint8_t rst)
{
  // Data member initialization
  lcd->_dc = d_c;
  lcd->_cs = cs;
  lcd->_rst = rst;
  lcd->control_port = control_port;

  // Bus and buffer initialization
  spi_master_initialize(spi_port, mosi, miso, sck, ss);
  set_pin_mode(lcd->control_port, lcd->_dc, OUTPUT);
  set_pin_mode(lcd->control_port, lcd->_cs, OUTPUT);
  set_pin_mode(lcd->control_port, lcd->_rst, OUTPUT);
  set_pins(lcd->control_port, _BV(lcd->_dc) | _BV(lcd->_cs) | _BV(lcd->_rst));
  
  // Initial reset
  n5110_reset(lcd);
}

void n5110_reset(struct Nokia5110LCD *lcd)
{
  // Toggle reset
  clear_pin(lcd->control_port, lcd->_rst);
  _delay_ms(10);
  set_pin(lcd->control_port, lcd->_rst);

  // Set screen options
  n5110_send_command(lcd, N5110_EXTENDED);  
  n5110_send_command(lcd, N5110_BIAS | N5110_DEFAULT_BIAS);
  n5110_send_command(lcd, N5110_CONTRAST | N5110_DEFAULT_CONTRAST); 
  n5110_send_command(lcd, N5110_BASIC);
  n5110_send_command(lcd, N5110_INVERSE);
  n5110_clear(lcd); 
}

void n5110_send_command(const struct Nokia5110LCD *lcd, uint8_t cmd)
{
  // Enter command mode, assert chip select
  clear_pin(lcd->control_port, lcd->_dc);
  clear_pin(lcd->control_port, lcd->_cs);
  spi_write(cmd);
  // Clear chip select
  set_pin(lcd->control_port, lcd->_cs);
}

void n5110_send_data(const struct Nokia5110LCD *lcd, uint8_t cmd)
{
  // Enter data mode, assert chip select
  set_pin(lcd->control_port, lcd->_dc);
  clear_pin(lcd->control_port, lcd->_cs);
  spi_write(cmd);
  // Clear chip select
  set_pin(lcd->control_port, lcd->_cs);
}

void n5110_clear(struct Nokia5110LCD *lcd)
{
  n5110_display_screen(lcd, NULL);
}

void n5110_display_screen(const struct Nokia5110LCD *lcd, const struct Bitmap2D *bitmap)
{
  n5110_send_command(lcd, N5110_SET_X | 0x00);
  n5110_send_command(lcd, N5110_SET_Y | 0x00);
  if (bitmap != NULL) {
    for (uint8_t y = 0; y < N5110_HEIGHT/8; y++) {
      for (uint8_t x = 0; x < N5110_WIDTH; x++) {
        n5110_send_data(lcd, bitmap_get_byte(bitmap, x, y*8));
      }
    }
  } else {
    for (uint8_t y = 0; y < N5110_HEIGHT/8; y++) {
      for (uint8_t x = 0; x < N5110_WIDTH; x++) {
        n5110_send_data(lcd, 0);
      }
    }
  }
}


static void spi_master_initialize(volatile uint8_t *spi_port, uint8_t mosi, 
    uint8_t miso, uint8_t sck, uint8_t ss)
{
  set_pin_mode(spi_port, mosi, OUTPUT);
  set_pin_mode(spi_port, miso, INPUT);
  set_pin_mode(spi_port, sck, OUTPUT);
  set_pin_mode(spi_port, ss, OUTPUT);
  set_pin(spi_port, ss);
  SPSR |= _BV(SPI2X);
  SPCR = _BV(SPE);
  SPCR |= _BV(MSTR);
}

static inline void spi_write(uint8_t data)
{
  SPDR = data;
  _delay_us(1);
}

/*static uint8_t spi_get_clk_div_mask(uint8_t target_mbits)zmz
{
  uint8_t clk_div = 2;
  uint8_t clk_div_mask = 0;
  while (F_CPU/clk_div > N5110_MAX_MBITS && clk_div > 0) {
    clk_div <<= 1;
  }

  // The following are clock divider bits obtained from the datasheet
  switch (clk_div) 
  {
    case 2:
      clk_div_mask |= _BV(SPI2X);
      break;
    case 4:
      break;
    case 8:
      clk_div_mask |= _BV(SPI2X) | _BV(SPR0);
      break;
    case 16:
      clk_div_mask |= _BV(SPR0);
      break;
    case 32:
      clk_div_mask |= _BV(SPI2X) | _BV(SPR1);
      break;
    case 64:
      clk_div_mask |= _BV(SPI2X) | _BV(SPR1) | _BV(SPR0);
      break;
    case 128:
      clk_div_mask |= _BV(SPR1) | _BV(SPR0);
      break;
    default: // F_CPU/4
      break;
  }
  return clk_div_mask;
}
*/
