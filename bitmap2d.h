#ifndef BITMAP2D_H_
#define BITMAP2D_H_
#include <stdint.h>

/* 
 * Struct Bitmap2D
 * Simulates a glorified 2D-array of bits.  Maximum size is 255x255
 * Members:
 *  *buf: Data block containing the bits
 *  width: Width, in bits
 *  height: Height, in bits
 */
struct Bitmap2D
{
  uint8_t *buf;
  uint8_t width;
  uint8_t height;
};

/*
 * Initialize the bitmap structure for width x height bits. Must be called
 * before any other function, except for bitmap_copy. If you wish to simply
 * copy a bitmap struct into a new one,  then initialize the new struct
 * with a width and height of 0. */
void bitmap_initialize(struct Bitmap2D *bitmap, uint8_t width, uint8_t height);

/* 
 * Free a bitmaps held data */
void bitmap_destroy(struct Bitmap2D *bitmap);

/*
 * Get the area size of a bitmap, in number of bits */
uint16_t bitmap_size(const struct Bitmap2D *bitmap);

/* 
 * The following are all simply Bitmap2D manipulation functions. They're
 * pretty self explanatory */
void bitmap_set_bit(struct Bitmap2D *bitmap, uint8_t x, uint8_t y);
void bitmap_set_range(struct Bitmap2D *bitmap, uint8_t x_init, uint8_t x_final, 
    uint8_t y_init, uint8_t y_final);
void bitmap_set_all(struct Bitmap2D *bitmap);

void bitmap_clear_bit(struct Bitmap2D *bitmap, uint8_t x, uint8_t y);
void bitmap_clear_range(struct Bitmap2D *bitmap, uint8_t x_init, uint8_t x_final, 
    uint8_t y_init, uint8_t y_final);
void bitmap_clear_all(struct Bitmap2D *bitmap);

uint8_t bitmap_get_bit(const struct Bitmap2D *bitmap, uint8_t x, uint8_t y);

/* 
 * IMPORTANT: See the note in bitmap_initialize */
void bitmap_copy(struct Bitmap2D* destination, const struct Bitmap2D *source);

#endif
