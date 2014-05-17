#ifndef PINMANIP_H_
#define PINMANIP_H_

#include <stdint.h>

typedef enum {INPUT, OUTPUT} PinMode;
inline volatile uint8_t* port_to_dir_reg(volatile uint8_t *port_reg);
inline volatile uint8_t* port_to_pin_reg(volatile uint8_t *port_reg);
inline void set_pin_mode(volatile uint8_t *port_reg, uint8_t pin, PinMode mode);

inline uint8_t get_bit(uint8_t src, uint8_t bit_i);
inline void set_bit(uint8_t *src, uint8_t bit_i);
inline void clear_bit(uint8_t *src, uint8_t bit_i);
inline void set_bits(uint8_t *src, uint8_t bit_set_mask);
inline void clear_bits(uint8_t *src, uint8_t bit_clear_mask);
inline uint8_t apply_mask(uint8_t value, uint8_t mask);

inline volatile uint8_t* port_to_dir_reg(volatile uint8_t *port_reg)
{
  return port_reg + 0x02;
}

inline volatile uint8_t* port_to_pin_reg(volatile uint8_t *port_reg)
{
  return port_reg + 0x01;
}
inline void set_pin_mode(volatile uint8_t *port_reg, uint8_t pin, PinMode mode)
{
  if (mode == INPUT) {
      set_bit((uint8_t*)port_reg, pin);
      clear_bit((uint8_t*)port_to_dir_reg(port_reg), pin);
  } else {
      set_bit((uint8_t*)port_to_dir_reg(port_reg), pin);
  }
}

inline uint8_t get_bit(uint8_t src, uint8_t bit_i) 
{ 
    return (~src & (1 << bit_i)) > 0; 
}

inline void set_bit(uint8_t *src, uint8_t bit_i)
{
  *src |= 1 << bit_i;
}

inline void clear_bit(uint8_t *src, uint8_t bit_i)
{
  *src &= ~(1 << bit_i);
}

inline void set_bits(uint8_t *src, uint8_t bit_set_mask)
{
  *src |= bit_set_mask;
}

inline void clear_bits(uint8_t *src, uint8_t bit_clear_mask)
{
  *src &= ~bit_clear_mask;
}

inline uint8_t apply_mask(uint8_t value, uint8_t mask)
{
  return value & mask;
}



#endif
