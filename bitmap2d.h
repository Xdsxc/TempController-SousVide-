#ifndef BITMAP2D_H_
#define BITMAP2D_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
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
  uint8_t *_buf;
  uint16_t _width;
  uint16_t _height;
};


/* When using this bitmap, any call that creates a copy (specified by the
 * make_copy parameter) will dynamically allocate memory. It is the callers
 * responsibility for managing the block before changing buffers */


void bitmap_initialize(struct Bitmap2D *bitmap);
/* 
 * Free a bitmaps held data. Only for bitmaps not referencing an external
 * buffer */
void bitmap_free_buffer(struct Bitmap2D *bitmap);

/* Allocate a new buffer, initialized to zeroes. Must already be initialized
 * with a set size */
bool bitmap_new_buffer(struct Bitmap2D *bitmap);

bool bitmap_set_buffer(struct Bitmap2D *bitmap, uint8_t *buf, uint16_t width, uint16_t height, bool make_copy);

/* Only for bitmaps with dynamically allocated memory, or freshly initialized*/
bool bitmap_resize(struct Bitmap2D *bitmap, uint16_t width, uint16_t height);

/*
 * Get the area size of a bitmap, in number of bits */
uint16_t bitmap_size(const struct Bitmap2D *bitmap);

/* 
 * The following are all simply Bitmap2D manipulation functions. They're
 * pretty self explanatory */
void bitmap_set_bit(struct Bitmap2D *bitmap, uint16_t x, uint16_t y);
void bitmap_set_range(struct Bitmap2D *bitmap, uint16_t x_init, uint16_t x_final, 
    uint16_t y_init, uint16_t y_final);
void bitmap_set_all(struct Bitmap2D *bitmap);

void bitmap_clear_bit(struct Bitmap2D *bitmap, uint16_t x, uint16_t y);
void bitmap_clear_range(struct Bitmap2D *bitmap, uint16_t x_init, uint16_t x_final, 
    uint16_t y_init, uint16_t y_final);
void bitmap_clear_all(struct Bitmap2D *bitmap);

uint8_t bitmap_get_bit(const struct Bitmap2D *bitmap, uint16_t x, uint16_t y);
uint8_t bitmap_get_byte(const struct Bitmap2D *bitmap, uint16_t x, uint16_t y);
void bitmap_write_byte(struct Bitmap2D *bitmap, uint16_t x, uint16_t y, uint8_t val);


void bitmap_superimpose(struct Bitmap2D *source, uint16_t src_x, uint16_t src_y, uint16_t width,
                        struct Bitmap2D *dest  , uint16_t dest_x, uint16_t dest_y);

#endif
