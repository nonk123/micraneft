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

  tile_t* copy = copy_world(world);

  get_console_window_size(&rows, &cols);

  if (extra_buffer == NULL)
    extra_buffer = calloc(rows * cols, sizeof(char));
  else
    extra_buffer = realloc(extra_buffer, rows * cols * sizeof(char));

  x0 = center_x - cols / 2;
  y0 = center_y - rows / 2;

  for (y = 0; y < rows; y++)
    {
      int tile_y = y0 + y;

      for (x = 0; x < cols; x++)
        {
          char* in_buffer = &extra_buffer[rows * x + y];

          int tile_x = x0 + x;

          if (tile_y >= 0 && tile_x >= 0
              && tile_y < world->height && tile_x < world->width)
            {
              char ours = get_tile(copy, tile_x, tile_y, world->width, world->height)->displayed_as;

              if (ours != *in_buffer)
                {
                  /* The entire world is upside-down. */
                  move_cursor_to(x, rows - 1 - y);
                  putchar(ours);
                  *in_buffer = ours;
                }
            }
          else
            {
              move_cursor_to(x, rows - 1 - y);
              putchar(' ');
              *in_buffer = ' ';
            }
        }
    }

  /* Prevent jitteriness. */
  move_cursor_to(0, 0);

  free(copy);
}
