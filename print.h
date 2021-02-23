#ifndef PRINT_H
#define PRINT_H

#include "world.h"

typedef struct frame_t {
  world_t* world;
  int center_x, center_y; /* center on this tile in the world */
  tile_t* buffer; /* always set to NULL when initializing */
  int rows, cols; /* filled automatically */
  int fps;
} frame_t;

void print_frame(frame_t frame);

#endif /* PRINT_H */
