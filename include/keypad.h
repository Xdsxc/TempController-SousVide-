#ifndef KEYPAD_H_
#define KEYPAD_H_
#include <stdint.h>

char read_keypad(void);
uint8_t keypad_char_to_hex(const char x);
char read_keypad_column(uint8_t col_index);

#endif
