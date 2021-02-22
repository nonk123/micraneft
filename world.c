#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#include "console.h"

world_t generate_world(int width, int height)
{
  tile_t* buffer = calloc(width * height, sizeof(tile_t));

  tile_t empty = {' '};
  tile_t block = {'#'};

  tile_t head = {'o'};
  tile_t body = {'A'};

  entity_t* player = malloc(sizeof(entity_t));
  world_t world = {buffer, player, width, height, width * height};

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

  player->parts = calloc(2, sizeof(tile_t));
  player->parts[0] = head;
  player->parts[1] = body;

  player->next = NULL;

  return world;
}

int is_opaque(tile_t* tile)
{
  return tile->displayed_as != ' ';
}

tile_t* get_tile(tile_t* tiles, int x, int y, int width, int height)
{
  int y_flipped = height - 1 - y;
  return &tiles[width * y_flipped + x];
}

tile_t* get_world_tile(world_t* world, int x, int y)
{
  return get_tile(world->tiles, x, y, world->width, world->height);
}

tile_t* get_entity_tile(entity_t* entity, int x, int y)
{
  return get_tile(entity->parts, x, y, entity->width, entity->height);
}
