#include "world.h"

#include <stdio.h>
#include <stdlib.h>

struct world generate_world(int width, int height)
{
  struct tile* buffer = calloc(width * height, sizeof(struct tile));

  struct tile empty = {' '};
  struct tile block = {'#'};

  struct world world = {buffer, width, height};

  int x, y;

  for (x = 0; x < width; x++)
    {
      for (y = 0; y < height / 2; y++)
        *get_tile(&world, x, y) = empty;

      for (; y < height; y++)
        *get_tile(&world, x, y) = block;
    }

  return world;
}

void print_world(struct world* world)
{
  int x, y;

  for (y = 0; y < world->height; y++)
    {
      if (y > 0)
        putchar('\n');

      for (x = 0; x < world->width; x++)
        putchar(get_tile(world, x, y)->displayed_as);
    }
}

struct tile* get_tile(struct world* world, int x, int y)
{
  int index = world->height * x + y;
  return &world->tiles[index];
}
