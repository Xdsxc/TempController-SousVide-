#include <avr/io.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <util/delay_basic.h>
#include "keypad.h"
static const char keypad_char_array[4][4] = {{ '1', '4', '7', '*'},
                                             { '2', '5', '8', '0'},
                                             { '3', '6', '9', '#'},
                                             { 'A', 'B', 'C', 'D'}};

static inline char _read_keypad_column(uint8_t col_index)
{
  uint8_t first_set_bit;
  PORTC = 0xFF & ~_BV(4 + col_index);
  _delay_loop_1(5);
  first_set_bit = ffs((~PINC) & 0x0F);
  return first_set_bit == 0 ? '\0' : keypad_char_array[col_index][first_set_bit - 1];
}

char read_keypad(void)
{
  DDRC = 0xF0;
  PORTC = 0x0F;
  char col_read_char;
  for (uint8_t i = 0; i < 4; i++) {
      col_read_char = _read_keypad_column(i);
      if (col_read_char != '\0') {
          break;
      }
  }
  return col_read_char;
}

uint8_t keypad_char_to_hex(const char x)
{
  if (x == '\0') {
      return 0x1F;
  } else if (isdigit(x)) {
      return x - '0';
  } else if (isalpha(x)) {
      return x - '7';
  } else if (x == '*') {
      return 0x0E;
  } else if (x == '#') {
      return 0x0F;
  } else { // Error condition
      return 0x1B;
  }
}

char read_keypad_column(uint8_t col_index)
{
  return _read_keypad_column(col_index);
}
