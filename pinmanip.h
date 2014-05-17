#ifndef PINMANIP_H_
#define PINMANIP_H_
#include <stdint.h>

#ifndef INPUTS_PULLED_DOWN
inline uint8_t getBit(uint8_t src, uint8_t bit_i) 
{ 
    return (~src & (1 << bit_i)) > 0; 
}

#else
inline uint8_t getBit(uint8_t src, uint8_t bit_i) 
{ 
    return (src & (1 << bit_i)) > 0; 
}
#endif

inline void setBit(uint8_t* src, uint8_t bit_i)
{
  *src |= 1 << bit_i;
}

inline void clearBit(uint8_t* src, uint8_t bit_i)
{
  *src &= ~(1 << bit_i);
}

inline void setBits(uint8_t* src, uint8_t bit_set_mask)
{
  *src |= bit_set_mask;
}

inline void clearBits(uint8_t* src, uint8_t bit_clear_mask)
{
  *src &= ~bit_clear_mask;
}

inline uint8_t applyMask(uint8_t value, uint8_t mask)
{
  return value & mask;
}
#endif
