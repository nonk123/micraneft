#include <stdlib.h>
#include <stdio.h>

#include "world.h"
#include "console.h"

void print_world(struct world* world)
{
  int x, y;
  int rows, cols;

  int tiles_size = world->area * sizeof(struct tile);

  /* The tiles array is copied in order to reflect the entities inside it. */
  struct tile* copy = malloc(tiles_size);
  struct entity* node = world->entities;

  int x0, y0;

  memcpy(copy, world->tiles, tiles_size);

  get_console_window_size(&rows, &cols);

  x0 = node->ix - cols / 2;
  y0 = node->iy - rows / 2 + 1; /* account for the head */

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

  move_cursor_to(0, 0);

  /* Print upside-down to make sure Y = 0 is at the bottom of the screen. */
  for (y = rows - 1; y >= 0; y--)
    {
      int tile_y = y0 + y;

      for (x = 0; x < cols; x++)
        {
          int tile_x = x0 + x;

          if (tile_y >= 0 && tile_x >= 0
              && tile_y < world->height && tile_x < world->width)
            {
              struct tile* tile = get_tile(copy, tile_x, tile_y, world->width, world->height);
              putchar(tile->displayed_as);
            }
          else
            putchar(' ');
        }
    }

  /* Prevent the screen from jumping around. */
  move_cursor_to(0, 0);

  free(copy);
}
