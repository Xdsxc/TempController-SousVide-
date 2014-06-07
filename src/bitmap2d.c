#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitmap2d.h"
#include <math.h>

static inline uint16_t coordinates_to_index(uint16_t x, uint16_t y, uint16_t width)
{
  return x + ((y/8)*width);
}

/* Initialize bitmap */
void bitmap_initialize(struct Bitmap2D *bitmap)
{
  bitmap->_buf = NULL;
}

/* Free bitmaps associated buffer. DYNAMIC ONLY */
void bitmap_free_buffer(struct Bitmap2D *bitmap)
{
  free(bitmap->_buf);
  bitmap->_buf = NULL;
}

/* Get bitmaps area */
uint16_t bitmap_size(const struct Bitmap2D *bitmap)
{
  return bitmap->_width * bitmap->_height;
}

/* Set a single bit in the bitmap */
void bitmap_set_bit(struct Bitmap2D *bitmap, uint16_t x, uint16_t y)
{
  bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)] |= 1 << (y%8);
}

/* Set a range of bits in the bitmap */
void bitmap_set_range(struct Bitmap2D *bitmap, uint16_t x_init, uint16_t x_final, 
    uint16_t y_init, uint16_t y_final)
{
  if (x_init >= bitmap->_width || y_init >= bitmap->_height) {
    return;
  }
  if (x_final >= bitmap->_width) {
    x_final = bitmap->_width;
  }

  if (y_final >= bitmap->_height) {
    y_final = bitmap->_height;
  }

  for (; y_init <= y_final; y_init += 8) {
    if (y_init + 8 > y_final) {
      uint8_t bit_mask = 0xFF >> (y_final - y_init + 8);
      for (uint16_t x = x_init; x <= x_final; x++) {
        bitmap_write_byte(bitmap, x, y_init, bitmap_get_byte(bitmap, x, y_init) | bit_mask);
      }
    } else {
      for (uint16_t x = x_init; x <= x_final; x++) {
        bitmap_write_byte(bitmap, x, y_init, 0xFF);
      }
    }
  }
}

/* Clear a single bit */
void bitmap_clear_bit(struct Bitmap2D *bitmap, uint16_t x, uint16_t y)
{
  bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)] &= ~(1 << (y%8));
}

/* Clear a range of bits */
void bitmap_clear_range(struct Bitmap2D *bitmap, uint16_t x_init, uint16_t x_final, 
    uint16_t y_init, uint16_t y_final)
{
  if (x_init >= bitmap->_width || y_init >= bitmap->_height) {
    return;
  }
  if (x_final >= bitmap->_width) {
    x_final = bitmap->_width;
  }

  if (y_final >= bitmap->_height) {
    y_final = bitmap->_height;
  }
  for (; y_init <= y_final; y_init += 8) {
    if (y_init + 8 > y_final) {
      uint8_t bit_mask = ~(0xFF >> (y_final - y_init + 8));
      for (uint16_t x = x_init; x <= x_final; x++) {
        bitmap_write_byte(bitmap, x, y_init, bitmap_get_byte(bitmap, x, y_init) & bit_mask);
      }
    } else {
      for (uint16_t x = x_init; x <= x_final; x++) {
        bitmap_write_byte(bitmap, x, y_init, 0x00);
      }
    }
  }
}

/* Get a single bit */
uint8_t bitmap_get_bit(const struct Bitmap2D *bitmap, uint16_t x, uint16_t y)
{
  return (bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)] & (1 << y%8)) > 0;
}

/* Get a single byte in the bitmap */
uint8_t bitmap_get_byte(const struct Bitmap2D *bitmap, uint16_t x, uint16_t y)
{
  uint8_t byte_offset = y%8;
  if (byte_offset == 0) {
    return bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)];
  } else {
    uint8_t byte_lower = bitmap->_buf[coordinates_to_index(x, y + 1, bitmap->_width)];
    uint8_t byte_upper = bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)];
    return (byte_lower << byte_offset) | (byte_upper >> (8 - byte_offset));
  }
}

/* Write a byte in the bitmap. Allows writing in between rows as well */
void bitmap_write_byte(struct Bitmap2D *bitmap, uint16_t x, uint16_t y, uint8_t val) 
{
  uint8_t byte_offset = y%8;
  if (byte_offset == 0) {
    bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)] = val;
  } else {
    uint8_t byte_mask = (1 << byte_offset) - 1;
    bitmap->_buf[coordinates_to_index(x, y, bitmap->_width)] &= byte_mask | (val << byte_offset);
    bitmap->_buf[coordinates_to_index(x, y + 8, bitmap->_width)] &= ~byte_mask | (val >> (8 - byte_offset));
    if (y + 8 < bitmap->_height) {
      bitmap->_buf[coordinates_to_index(x, y + 8, bitmap->_width)] &= ~byte_mask | (val >> (8 - byte_offset));
    }
  }
}

/* Set the entire bitmap to all zeroes */
void bitmap_clear_all(struct Bitmap2D *bitmap)
{
  memset(bitmap->_buf, 0,(bitmap->_width*bitmap->_height)/8 + (bitmap->_width*bitmap->_height)%8);
}

/* Set the entire bitmap to all ones */
void bitmap_set_all(struct Bitmap2D *bitmap)
{
  memset(bitmap->_buf, 0xFF, (bitmap->_width*bitmap->_height)/8);
}

/* Dynamically allocate a new bitmap. Does not free the previous bitmap. It is
 * the callers responsibility to ensure that dynamic bitmaps are freed or
 * stored before changing the buffer.  */
bool bitmap_new_buffer(struct Bitmap2D *bitmap)
{
  bitmap->_buf = (uint8_t*)calloc((bitmap->_width*bitmap->_height)/8 + 1, sizeof(uint8_t));
  return bitmap->_buf != NULL;
}

/* Set a bitmaps source buffer. If make_copy, dynamically allocates a new array
 * and copies buf into the array, otherwise uses buf as the internal array. */
bool bitmap_set_buffer(struct Bitmap2D *bitmap, uint8_t *buf, uint16_t width, uint16_t height, bool make_copy)
{
  bitmap->_width = width;
  bitmap->_height = height;
  if (make_copy) {
    if ((bitmap->_buf = malloc((width*height)/8 + 1)) != NULL) {
      memcpy(bitmap->_buf, buf, bitmap_size(bitmap)/8 + 1);
      return true;
    } else {
      return false;
    }
  } else {
    bitmap->_buf = buf;
    return true;
  }
}

/* Resize a dynamically allocated buffer to width and height */
bool bitmap_resize(struct Bitmap2D *bitmap, uint16_t width, uint16_t height)
{
    bitmap->_width = width;
    bitmap->_height = height;
    bitmap->_buf = realloc(bitmap->_buf, (width*height)/8 + 1);
    return  bitmap->_buf != NULL;
}

/* Superimposes the source bitmap starting and src_x and src_y to the dest
 * bitmap starting at dest_x and dest_x */
void bitmap_superimpose(struct Bitmap2D *source, uint16_t src_x, uint16_t src_y, uint16_t width,
                        struct Bitmap2D *dest  , uint16_t dest_x, uint16_t dest_y)
{
  if (src_y >= source->_height || dest_y >= dest->_height) {
    return;
  } 

  if (src_x + width > source->_width) {
    width = src_x + width - source->_width;
  }

  if (dest_x + width > dest->_width) {
    width = dest_x + width - dest->_width;
  }

  for (uint16_t i = 0; i < width; i++) {
    bitmap_write_byte(dest, dest_x + i, dest_y, bitmap_get_byte(source, src_x + i, src_y));
  }
}



