#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include "console.h"

struct world generate_world(int width, int height)
{
  struct tile* buffer = calloc(width * height, sizeof(struct tile));

  struct tile empty = {' '};
  struct tile block = {'#'};

  struct tile head = {'o'};
  struct tile body = {'A'};

  struct entity* player = malloc(sizeof(struct entity));
  struct world world = {buffer, player, width, height, width * height};

  int x, y;

  for (x = 0; x < width; x++)
    {
      /* Fill the bottom half of the world. */
      for (y = 0; y < height / 2; y++)
        *get_world_tile(&world, x, y) = block;

      for (; y < height; y++)
        *get_world_tile(&world, x, y) = empty;
    }

  /* A wall on the left. */
  for (x = 0; x < 5; x++)
    for (y = 0; y < height; y++)
      *get_world_tile(&world, x, y) = block;

  /* A hole on the right. */
  for (x = 0; x < 5; x++)
    for (y = 0; y < height / 2; y++)
      *get_world_tile(&world, world.width - 16 - x, y) = empty;

  /* Spawn a crappy player. */

  player->x = (double) width / 2;
  player->y = (double) height / 2;

  player->ix = round(player->x);
  player->iy = round(player->y);

  player->vx = player->vy = 0.0;

  player->width = 1;
  player->height = 2;

  player->parts = calloc(2, sizeof(struct tile));
  player->parts[0] = head;
  player->parts[1] = body;

  player->next = NULL;

  return world;
}

int is_opaque(struct tile* tile)
{
  return tile->displayed_as != ' ';
}

struct tile* get_tile(struct tile* tiles, int x, int y, int width, int height)
{
  int y_flipped = height - 1 - y;
  return &tiles[width * y_flipped + x];
}

struct tile* get_world_tile(struct world* world, int x, int y)
{
  return get_tile(world->tiles, x, y, world->width, world->height);
}

struct tile* get_entity_tile(struct entity* entity, int x, int y)
{
  return get_tile(entity->parts, x, y, entity->width, entity->height);
}
