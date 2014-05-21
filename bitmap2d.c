#include <stdlib.h>
#include <string.h>
#include "bitmap2d.h"

static inline uint16_t coordinates_to_index(uint8_t x, uint8_t y, uint8_t width)
{
  return x + ((y/8)*width);
}

void bitmap_initialize(struct Bitmap2D *bitmap, uint8_t width, uint8_t height)
{
  bitmap->width = width;
  bitmap->height = height;
  if (width > 0 && height > 0) {
    bitmap->buf = (uint8_t*)calloc((width*height)/8 + 1, sizeof(uint8_t));
  } else {
    bitmap->buf = NULL;
  }
}

void bitmap_destroy(struct Bitmap2D *bitmap)
{
  free(bitmap->buf);
}

uint16_t bitmap_size(const struct Bitmap2D *bitmap)
{
  return bitmap->width * bitmap->height;
}

void bitmap_set_bit(struct Bitmap2D *bitmap, uint8_t x, uint8_t y)
{
  bitmap->buf[coordinates_to_index(x, y, bitmap->width)] |= 1 << (y%8);
}

void bitmap_set_range(struct Bitmap2D *bitmap, uint8_t x_init, uint8_t x_final, 
    uint8_t y_init, uint8_t y_final)
{
  for (uint8_t x = x_init; x <= x_final; x++) {
    for (uint8_t y = y_init; y <= y_final; y++) {
      bitmap->buf[coordinates_to_index(x, y, bitmap->width)] |= 1 << (y%8);
    }
  }
}

void bitmap_clear_bit(struct Bitmap2D *bitmap, uint8_t x, uint8_t y)
{
  bitmap->buf[coordinates_to_index(x, y, bitmap->width)] &= ~(1 << (y%8));
}

void bitmap_clear_range(struct Bitmap2D *bitmap, uint8_t x_init, uint8_t x_final, 
    uint8_t y_init, uint8_t y_final)
{
  for (uint8_t x = x_init; x <= x_final; x++) {
    for (uint8_t y = y_init; y <= y_final; y++) {
      bitmap->buf[coordinates_to_index(x, y, bitmap->width)] &= ~(1 << (y%8));
    }
  }
}

uint8_t bitmap_get_bit(const struct Bitmap2D *bitmap, uint8_t x, uint8_t y)
{
  return (bitmap->buf[coordinates_to_index(x, y, bitmap->width)] & (1 << y%8)) > 0;
}

void bitmap_clear_all(struct Bitmap2D *bitmap)
{
  memset(bitmap->buf, 0,(bitmap->width*bitmap->height)/8 + 1);
}

void bitmap_set_all(struct Bitmap2D *bitmap)
{
  memset(bitmap->buf, 1, (bitmap->width*bitmap->height)/8);
}

void bitmap_copy(struct Bitmap2D* destination, const struct Bitmap2D *source)
{
  if (destination->buf != NULL) {
    bitmap_destroy(destination);
  }  
  destination->buf = (uint8_t*)malloc((source->width*source->height)/8 + 1);
  destination->height = source->height;
  destination->width = source->width;
  memcpy(destination->buf, source->buf, (source->width*source->height)/8 + 1);
}
/*
void bitmap_set_x(struct Bitmap2D *bitmap, uint16_t x)
{
  bitmap_set_bit(
}*/
