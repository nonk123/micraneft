#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "world.h"
#include "console.h"

tile_t* copy_world(world_t* world)
{
  int x, y;

  int size = world->area * sizeof(tile_t);

  entity_t* node = world->entities;

  tile_t* copy = malloc(size);
  memcpy(copy, world->tiles, size);

  /* Project entities onto the copy. */
  while (node)
    {
      for (y = node->height - 1; y >= 0; y--)
        {
          int entity_y = node->iy + y;

          for (x = 0; x < node->width; x++)
            {
              int entity_x = node->ix + x;

              *get_tile(copy, entity_x, entity_y, world->width, world->height)
                = *get_entity_tile(node, x, y);
            }
        }
      node = node->next;
    }

  return copy;
}

void print_world(world_t* world, int center_x, int center_y)
{
  int x, y;
  int rows, cols;

  int x0, y0;

  static char* extra_buffer = NULL;
  static int buffer_size = 0;
  int old_buffer_size;

  tile_t* copy = copy_world(world);

  /* Magic values to trigger the check below. */
  int cursor_x = -2, cursor_y = 0;

  get_console_window_size(&rows, &cols);

  old_buffer_size = buffer_size;
  buffer_size = rows * cols * sizeof(char);

  extra_buffer = realloc(extra_buffer, buffer_size);

  /* Redraw when the window size changes. */
  if (old_buffer_size != buffer_size)
    memset(extra_buffer, 0, buffer_size);

  x0 = center_x - cols / 2;
  y0 = center_y - rows / 2;

  for (y = 0; y < rows; y++)
    {
      int tile_y = y0 + y;

      for (x = 0; x < cols; x++)
        {
          char* in_buffer = &extra_buffer[rows * x + y];
          char draw = 0; /* draw nothing by default */

          int tile_x = x0 + x;

          if (tile_y >= 0 && tile_x >= 0
              && tile_y < world->height && tile_x < world->width)
            draw = get_tile(copy, tile_x, tile_y, world->width, world->height)->displayed_as;
          else
            draw = ' ';

          if (draw && draw != *in_buffer)
            {
              /* putchar moves the cursor right anyways. */
              if (cursor_y != y || x - cursor_x > 1)
                /* The entire world is upside-down. */
                move_cursor_to(x, rows - 1 - y);

              putchar(draw);
              *in_buffer = draw;

              cursor_x = x;
              cursor_y = y;
            }
        }
    }

  /* Prevent jitteriness. */
  move_cursor_to(0, 0);

  free(copy);
}
