#ifndef ONEWIRE_DS18B20_H_
#define ONEWIRE_DS18B20_H_
#include <stdint.h>
#include "onewire.h"
// DS18B20 specific
enum {CONVERT_T = 0x44, WRITE_SCRATCHPAD = 0x4E, READ_SCRATCHPAD = 0xBE, 
    COPY_SCRATCHPAD = 0x48, RECALL_E2 = 0xB8, READ_POWER_SUPPLY = 0xB4};
typedef enum {LOW = 1, MED, HIGH} ThermResolution;
float ds18b20_get_temperature(struct OnewireDevice *therm, char* buf);
int8_t ds18b20_setup_device(struct OnewireDevice *therm, 
                            volatile uint8_t *bus_register, uint8_t bus_pin);
void ds18b20_set_resolution(struct OnewireDevice *therm, ThermResolution decimal_point_resolution);
uint8_t ds18b20_get_resolution(struct OnewireDevice *therm);
#endif
