#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

#define GRAVITY -10.0

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

void physics_tick(struct world* world, double delta)
{
  struct entity* node = world->entities;

  while (node)
    {
      int x, y;
      double x1, y1; /* x and y after applying velocity */

      node->vy += GRAVITY * delta;

      x1 = node->x + node->vx * delta;
      y1 = node->y + node->vy * delta;

      /* Run collision detection. */

      node->is_on_floor = 0;

      for (x = 0; x < node->width; x++)
        {
          struct tile* above = get_world_tile(world, node->ix + x, node->iy + node->height);
          struct tile* below = get_world_tile(world, node->ix + x, node->iy - 1);

          if ((is_opaque(above) && node->vy > 0.0)
              || (is_opaque(below) && node->vy < 0.0))
            {
              if (node->vy < 0.0)
                node->is_on_floor = 1;

              node->vy = 0.0;
              y1 = node->y;
              break;
            }
        }

      for (y = 0; y < node->height; y++)
        {
          struct tile* left = get_world_tile(world, node->ix - 1, node->iy + y);
          struct tile* right = get_world_tile(world, node->ix + node->width, node->iy + y);

          if ((is_opaque(left) && node->vx < 0.0)
              || (is_opaque(right) && node->vx > 0.0))
            {
              node->vx = 0.0;
              x1 = node->x;
              break;
            }
        }

      node->x = x1;
      node->y = y1;

      node->ix = round(node->x);
      node->iy = round(node->y);

      node = node->next;
    }
}

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
              _putch(tile->displayed_as);
            }
          else
            _putch(' ');
        }
    }

  /* Prevent the screen from jumping around. */
  move_cursor_to(0, 0);
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

/* Windows-specific functions (aka trash). */

HANDLE get_stdout()
{
  return GetStdHandle(STD_OUTPUT_HANDLE);
}

HANDLE get_stdin()
{
  return GetStdHandle(STD_INPUT_HANDLE);
}

void move_cursor_to(int x, int y)
{
  COORD coord;

  coord.X = x;
  coord.Y = y;

  SetConsoleCursorPosition(get_stdout(), coord);
}

void get_console_window_size(int* rows, int* cols)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(get_stdout(), &csbi);

  /* Not sure what's happening in the RHS. */
  *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
