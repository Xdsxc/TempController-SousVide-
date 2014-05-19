#include <stdint.h>
#include "onewire_ds18b20.h"
#include "onewire.h"
#include "pinmanip.h"

inline float celsius_to_fahrenheit(float degrees_celsius)
{
  return (1.8 * degrees_celsius) + 32;
}

/* TODO: Implement separate function that will not block while
         waiting for the conversion to finish.
         This may lead to an infinite loop if the device is
         unplugged mid-conversion. A timeout must also be implemented.*/
float ds18b20_get_temperature(struct OnewireDevice *therm)
{
  clear_pin(therm->bus_port, 7);
  onewire_send_reset_pulse(therm);
  onewire_send_byte(therm, SKIP_ROM);
  onewire_send_byte(therm, CONVERT_T);

  while (onewire_read_byte(therm) == 0);

  onewire_send_reset_pulse(therm);
  onewire_send_byte(therm, SKIP_ROM);
  onewire_send_byte(therm, READ_SCRATCHPAD);

  // TODO: Add check for negative celsius
  int8_t decimal_portion;
  int8_t decimal_portion_fixed = 0;
  float reading;
  decimal_portion = onewire_read_byte(therm);
  reading = ((decimal_portion& 0xF0) >> 4) | (onewire_read_byte(therm) << 4);
  decimal_portion &= 0x0F;

  for (int8_t i = therm->resolution; i >= 0; i--) {
    decimal_portion_fixed |= get_bit(decimal_portion, i) << (4 - i);
  }

  if (decimal_portion_fixed != 0) {
    reading += 1.0f/decimal_portion_fixed;
  }

  return reading;
}

int8_t ds18b20_setup_device(struct OnewireDevice *therm, 
                            volatile uint8_t *bus_register, uint8_t bus_pin)
{
  if (onewire_setup_device(therm, bus_register, bus_pin) == -1) {
    return -1;
  }
  therm->resolution = ds18b20_get_resolution(therm);
  return 1;
}

uint8_t ds18b20_get_resolution(struct OnewireDevice *therm)
{
  // Read configuration register - byte 4 of scratchpad
  onewire_send_reset_pulse(therm);
  onewire_send_byte(therm, SKIP_ROM);
  onewire_send_byte(therm, READ_SCRATCHPAD);
  onewire_read_byte(therm);
  onewire_read_byte(therm);
  onewire_read_byte(therm);
  onewire_read_byte(therm);
  int8_t configuration_register = onewire_read_byte(therm);
  // Device resolution are bits 5 and 6 of the configuration register
  return (configuration_register & 0x60) >> 5;
}

void ds18b20_set_resolution(struct OnewireDevice *therm, ThermResolution decimal_point_resolution)
{
  if (decimal_point_resolution > 4) return;

  // Resolution is specified as byte 2, bits[6:5] of the writable scratchpad
  onewire_send_reset_pulse(therm);
  onewire_send_byte(therm, SKIP_ROM);
  onewire_send_byte(therm, WRITE_SCRATCHPAD);
  onewire_send_byte(therm, 0x00);
  onewire_send_byte(therm, 0x00);
  onewire_send_byte(therm, decimal_point_resolution << 5);
  therm->resolution = decimal_point_resolution;
}
