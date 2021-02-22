#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#include "open-simplex-noise/open-simplex-noise.h"
#include "console.h"

/*
 * The percentage of world height occupied by hills.
 *
 * Everything below is taken up by a big chunk of ground.
 */
#define HILLS_HEIGHT 0.5

/* The height of the grass layer. */
#define GRASS_LAYER 2

#define NOISE_PERIOD 64

world_t generate_world(int width, int height)
{
  int x, y;

  tile_t* buffer = calloc(width * height, sizeof(tile_t));

  tile_t empty = {' ', 7, 0};

  tile_t grass = {'#', 10, 1};
  tile_t dirt  = {'#',  6, 1};

  tile_t head = {'o', 14, 1};
  tile_t body = {'A',  3, 1};

  entity_t* player = malloc(sizeof(entity_t));
  world_t world = {buffer, player, width, height, width * height};

  struct osn_context* osn;

  open_simplex_noise(time(NULL), &osn);

  for (x = 0; x < width; x++)
    {
      /* 1D noise. */
      double noise = open_simplex_noise2(osn, (double) x / NOISE_PERIOD, 0.0);
      double positive = 0.5 * (noise + 1.0);

      int bottom = height * (1.0 - HILLS_HEIGHT);
      int top = height * HILLS_HEIGHT;

      int result = bottom + top * positive;

      for (y = 0; y < result - GRASS_LAYER; y++)
        *get_world_tile(&world, x, y) = dirt;

      for (; y < result; y++)
        *get_world_tile(&world, x, y) = grass;

      /* Spawn the player in the center of the map. */
      if (x == width / 2)
        player->y = y;

      for (; y < height; y++)
        *get_world_tile(&world, x, y) = empty;
    }

  open_simplex_noise_free(osn);

  /* Spawn a crappy player. */

  player->x = (double) width / 2;

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
  return a->character == b->character && a->color == b->color;
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
