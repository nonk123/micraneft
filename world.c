#include "world.h"

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "open-simplex-noise/open-simplex-noise.h"

static struct {
  tile_t* tiles;
  entity_t* entities; /* a linked list */
} world = {NULL, NULL};

void initialize_player(entity_t* player)
{
  tile_t head = {'o', BACKGROUND, LIGHT_YELLOW};
  tile_t body = {'A', BACKGROUND, BLUE};

  player->is_player = 1;

  player->x = (double) WORLD_WIDTH / 2;
  /* Assumes the Y has already been set. */

  player->ix = round(player->x);
  player->iy = round(player->y);

  player->vx = 0.0;
  player->vy = 0.0;
  player->climb = 0.0;

  player->width = 1;
  player->height = 2;

  player->parts = calloc(2, sizeof(tile_t));
  player->parts[0] = head;
  player->parts[1] = body;

  player->next = NULL;
}

void generate_world()
{
  int x, y, i;

  tile_t* buffer = calloc(WORLD_WIDTH * WORLD_HEIGHT, sizeof(tile_t));
  entity_t* player = malloc(sizeof(entity_t));

  /* TODO: free previous tiles and entities. */

  world.tiles = buffer;
  world.entities = player;

  struct osn_context* osn;

  open_simplex_noise(time(NULL), &osn);

  for (x = 0; x < WORLD_WIDTH; x++)
    {
      /* 1D noise. */
      double noise = open_simplex_noise2(osn, (double) x / NOISE_PERIOD, 0.0);
      double positive = 0.5 * (noise + 1.0);

      int height = GROUND_HEIGHT + HILLS_HEIGHT * positive;

      for (y = 0; y < height - DIRT_LAYER - GRASS_LAYER; y++)
        *get_tile(x, y) = stone_tile;

      for (i = 0; i < DIRT_LAYER; i++, y++)
        *get_tile(x, y) = dirt_tile;

      for (i = 0; i < GRASS_LAYER; i++, y++)
        *get_tile(x, y) = grass_tile;

      /* Spawn the player in the center of the map. */
      if (x == WORLD_WIDTH / 2)
        player->y = y;

      for (; y < WORLD_HEIGHT; y++)
        *get_tile(x, y) = empty_tile;
    }

  open_simplex_noise_free(osn);
  initialize_player(player);
}

entity_t* get_entities()
{
  return world.entities;
}

int are_tiles_equal(tile_t* a, tile_t* b)
{
  return a->character == b->character && a->bg == b->bg && a->fg == b->fg;
}

int is_occupied(tile_t* tile)
{
  return !are_tiles_equal(tile, &empty_tile);
}

int is_impassable(tile_t* tile, entity_t* for_target)
{
  return is_occupied(tile)
    && !(tile->properties & CLIMBABLE)
    && !((tile->properties & DOOR) && for_target->is_player);
}

int is_cursor_in_range(int x, int y)
{
  return pow(x, 2.0) + pow(y, 2.0) <= 49.0;
}

int can_place_tile_at(int x, int y)
{
  entity_t* node = world.entities;

  if (is_occupied(get_tile(x, y)))
    return 0;

  /* Ensure it's not occupied by an entity. */
  while (node != NULL)
    {
      if (x >= node->ix && y >= node->iy
          && x < node->ix + node->width && y < node->iy + node->height)
        return 0;

      node = node->next;
    }

  if (x > 0 && y > 0 && x < WORLD_WIDTH - 1 && y < WORLD_HEIGHT - 1)
    {
      /* Check if the tile is connected to another tile. */
      tile_t* left  = get_tile(x - 1, y);
      tile_t* right = get_tile(x + 1, y);
      tile_t* below = get_tile(x, y - 1);
      tile_t* above = get_tile(x, y + 1);

      if (!is_occupied(left) && !is_occupied(right)
          && !is_occupied(above) && !is_occupied(below))
        return 0;
    }

  return 1;
}

void place_tile_at(int x, int y, tile_t tile)
{
  if (can_place_tile_at(x, y))
    *get_tile(x, y) = tile;
}

void remove_tile_at(int x, int y)
{
  if (x >= 0 && y >= 0 && x < WORLD_WIDTH && y < WORLD_HEIGHT)
    *get_tile(x, y) = empty_tile;
}

tile_t* get_tile(int x, int y)
{
  return &world.tiles[WORLD_WIDTH * y + x];
}

tile_t* get_part(entity_t* entity, int x, int y)
{
  int y_flipped = entity->height - 1 - y;
  return &entity->parts[entity->width * y_flipped + x];
}
