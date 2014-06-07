#ifndef STATE_MACHINES_H_
#define STATE_MACHINES_H_
#include <stdint.h>

int8_t SM_DisplayController(int8_t state);
int8_t SM_TemperatureController(int8_t state);
int8_t SM_MenuController(int8_t state);

#endif
