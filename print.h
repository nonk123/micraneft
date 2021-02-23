#ifndef PRINT_H
#define PRINT_H

#include "world.h"

typedef struct frame_t {
  world_t* world;
  entity_t* player; /* center everything around this entity */
  tile_t* buffer; /* always set to NULL when initializing */
  int rows, cols; /* filled automatically */
  /* Set the fields below manually. */
  int cursor_x, cursor_y;
  int fps;
} frame_t;

void print_frame(frame_t frame);

#endif /* PRINT_H */
