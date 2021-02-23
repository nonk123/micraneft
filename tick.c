#include <math.h>

#include "world.h"

#define DELTA 1.0 / 30.0

/*
 * Run collision detection on ENTITY.
 *
 * X1 and Y1 point to its final coordinates. They are adjusted in the process.
 */
void collide(world_t* world, entity_t* entity, double* x1, double* y1)
{
  int x, y;

  for (x = 0; x < entity->width; x++)
    {
      int x0 = entity->ix + x;

      tile_t* above = get_tile(world, x0, entity->iy + entity->height);
      tile_t* below = get_tile(world, x0, entity->iy - 1);

      if ((is_impassable(above, entity) && entity->vy > 0.0)
          || (is_impassable(below, entity) && entity->vy < 0.0))
        {
          if (entity->vy < 0.0)
            entity->is_on_floor = 1;

          entity->vy = 0.0;
          *y1 = entity->y;
          break;
        }
    }

  /* Traverse from top to bottom to make the check below work. */
  for (y = entity->height - 1; y >= 0; y--)
    {
      int y0 = entity->iy + y;

      tile_t* left = get_tile(world, entity->ix - 1, y0);
      tile_t* right = get_tile(world, entity->ix + entity->width, y0);

      if ((is_impassable(left, entity) && entity->vx < 0.0)
          || (is_impassable(right, entity) && entity->vx > 0.0))
        {
          tile_t* above = get_tile(world, entity->ix, entity->iy + entity->height);

          /* Move up one tile if there's enough space. */
          if (y == 0 && !is_impassable(above, entity))
            *y1 += 1;
          else
            {
              entity->vx = 0.0;
              *x1 = entity->x;
            }

          break;
        }
    }

  /* Check if we are on top of any climbable tiles. */
  for (y = 0; y < entity->height; y++)
    for (x = 0; x < entity->width; x++)
      {
        tile_t* tile = get_tile(world, entity->ix + x, entity->iy + y);

        if (tile->properties & CLIMBABLE)
          {
            tile_t* below = get_tile(world, entity->ix + x, entity->iy - 1);

            entity->vy = 0.0;
            *y1 = entity->y;

            if (is_impassable(below, entity) && entity->climb < 0.0)
              break;

            *y1 += entity->climb * DELTA;
            break;
          }
      }
}

#define GRAVITY -12.4

/* Used to determine close-to-zero coordinates. */
#define EPSILON 0.01

void physics_tick(world_t* world)
{
  entity_t* node = world->entities;

  while (node)
    {
      double x1, y1;

      /* Acceleration is m * s^(-2) */
      node->vy += GRAVITY * DELTA;

      /* Apply movement. */
      x1 = node->x + node->vx * DELTA;
      y1 = node->y + node->vy * DELTA;

      /* Run collision detection. */
      node->is_on_floor = 0;
      collide(world, node, &x1, &y1);

      /* Prevent going out of the rendering space. */

      if (x1 < EPSILON)
        x1 = EPSILON;
      else if (x1 > WORLD_WIDTH - 1 - EPSILON)
        x1 = WORLD_WIDTH - 1 - EPSILON;

      if (y1 < EPSILON)
        {
          /* The bottom of the world is still a floor. */
          node->is_on_floor = 1;
          y1 = EPSILON;
        }
      else if (y1 > WORLD_HEIGHT - 1 - EPSILON)
        y1 = WORLD_HEIGHT - 1 - EPSILON;

      /* Finalize the changes. */

      node->x = x1;
      node->y = y1;

      node->ix = round(node->x);
      node->iy = round(node->y);

      node = node->next;
    }
}
