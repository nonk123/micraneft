#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#include "open-simplex-noise/open-simplex-noise.h"
#include "console.h"

world_t generate_world()
{
  int x, y, i;

  tile_t* buffer = calloc(WORLD_WIDTH * WORLD_HEIGHT, sizeof(tile_t));

  tile_t head = {'o', BLACK, LIGHT_YELLOW};
  tile_t body = {'A', BLACK, LIGHT_BLUE};

  entity_t* player = malloc(sizeof(entity_t));
  world_t world = {buffer, player};

  struct osn_context* osn;

  open_simplex_noise(time(NULL), &osn);

  for (x = 0; x < WORLD_WIDTH; x++)
    {
      /* 1D noise. */
      double noise = open_simplex_noise2(osn, (double) x / NOISE_PERIOD, 0.0);
      double positive = 0.5 * (noise + 1.0);

      int height = GROUND_HEIGHT + HILLS_HEIGHT * positive;

      for (y = 0; y < height - DIRT_LAYER - GRASS_LAYER; y++)
        *get_world_tile(&world, x, y) = stone_tile;

      for (i = 0; i < DIRT_LAYER; i++, y++)
        *get_world_tile(&world, x, y) = dirt_tile;

      for (i = 0; i < GRASS_LAYER; i++, y++)
        *get_world_tile(&world, x, y) = grass_tile;

      /* Spawn the player in the center of the map. */
      if (x == WORLD_WIDTH / 2)
        player->y = y;

      for (; y < WORLD_HEIGHT; y++)
        *get_world_tile(&world, x, y) = sky_tile;
    }

  open_simplex_noise_free(osn);

  /* Spawn a crappy player. */

  player->x = (double) WORLD_WIDTH / 2;

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

int tiles_equal_p(tile_t* a, tile_t* b)
{
  return a->character == b->character && a->bg == b->bg && a->fg == b->fg;
}

int tile_opaque_p(tile_t* tile)
{
  return !tiles_equal_p(tile, &empty_tile) && !tiles_equal_p(tile, &sky_tile);
}

int can_place_tile_at(world_t* world, int x, int y)
{
  entity_t* node = world->entities;

  if (tile_opaque_p(get_world_tile(world, x, y)))
    return 0;

  while (node != NULL)
    {
      int rx = node->ix - x;
      int ry = node->iy - y;

      if (rx >= 0 && ry >= 0 && rx < node->width && rx < node->height)
        return 0;

      node = node->next;
    }

  if (x > 0 && y > 0 && x < WORLD_WIDTH - 1 && y < WORLD_HEIGHT - 1)
    {
      /* Check if the tile is connected to another tile. */
      tile_t* left = get_world_tile(world, x - 1, y);
      tile_t* right = get_world_tile(world, x + 1, y);
      tile_t* below = get_world_tile(world, x, y - 1);
      tile_t* above = get_world_tile(world, x, y + 1);

      if (!tile_opaque_p(left) && !tile_opaque_p(right)
          && !tile_opaque_p(above) && !tile_opaque_p(below))
        return 0;
    }

  return 1;
}

void place_tile_at(world_t* world, int x, int y, tile_t tile)
{
  if (can_place_tile_at(world, x, y))
    *get_world_tile(world, x, y) = tile;
}

void remove_tile_at(world_t* world, int x, int y)
{
  if (x >= 0 && y >= 0 && x < WORLD_WIDTH && y < WORLD_HEIGHT)
    *get_world_tile(world, x, y) = sky_tile;
}

int cursor_in_range(int x, int y)
{
  return pow(x, 2.0) + pow(y, 2.0) <= 49.0;
}

tile_t* get_tile(tile_t* tiles, int x, int y, int width, int height)
{
  int y_flipped = height - 1 - y;
  return &tiles[width * y_flipped + x];
}

tile_t* get_world_tile(world_t* world, int x, int y)
{
  return get_tile(world->tiles, x, y, WORLD_WIDTH, WORLD_HEIGHT);
}

tile_t* get_entity_tile(entity_t* entity, int x, int y)
{
  return get_tile(entity->parts, x, y, entity->width, entity->height);
}
