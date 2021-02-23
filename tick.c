#include <math.h>

#include "world.h"

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

      tile_t* above = get_world_tile(world, x0, entity->iy + entity->height);
      tile_t* below = get_world_tile(world, x0, entity->iy - 1);

      if ((tile_opaque_p(above) && entity->vy > 0.0) || (tile_opaque_p(below) && entity->vy < 0.0))
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

      tile_t* left = get_world_tile(world, entity->ix - 1, y0);
      tile_t* right = get_world_tile(world, entity->ix + entity->width, y0);

      if ((tile_opaque_p(left) && entity->vx < 0.0) || (tile_opaque_p(right) && entity->vx > 0.0))
        {
          tile_t* above = get_world_tile(world, entity->ix, entity->iy + entity->height);

          /* Move up one tile if there's enough space. */
          if (y == 0 && !tile_opaque_p(above))
            *y1 += 1;
          else
            {
              entity->vx = 0.0;
              *x1 = entity->x;
            }

          break;
        }
    }
}

#define GRAVITY -12.4

/* Used to determine close-to-zero coordinates. */
#define EPSILON 0.01

void physics_tick(world_t* world, double delta)
{
  entity_t* node = world->entities;

  while (node)
    {
      double x1, y1;

      /* Acceleration is m * s^(-2) */
      node->vy += GRAVITY * delta;

      /* Apply movement. */
      x1 = node->x + node->vx * delta;
      y1 = node->y + node->vy * delta;

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
