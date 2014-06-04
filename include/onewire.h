#ifndef AVR_ONEWIRE_H_
#define AVR_ONEWIRE_H_

#include <stdint.h>
#include <stdbool.h>
#define ONEWIRE_DEVICE_DISCONNECTED 255
struct OnewireDevice
{
  uint64_t crc : 8;
  uint64_t serial_no : 48;
  uint64_t family_code : 8;
  volatile uint8_t* bus_port;
  uint8_t bus_pin : 4;
  uint8_t resolution : 4;
};

// One-wire general
enum {SEARCH_ROM = 0xF0, READ_ROM = 0x33, MATCH_ROM = 0x55, SKIP_ROM = 0xCC, 
    ALARM_SEARCH = 0xEC};

bool onewire_setup_device(struct OnewireDevice *device, 
                            volatile uint8_t *bus_register, uint8_t bus_pin);
void onewire_send_byte(struct OnewireDevice *device, uint8_t byte);
uint8_t onewire_read_byte(struct OnewireDevice *device);
bool onewire_send_reset_pulse(struct OnewireDevice *device);
bool onewire_get_ROM_code(struct OnewireDevice *device);
#endif
