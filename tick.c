#include <math.h>

#include "world.h"

#define GRAVITY -10.0

/* Used to determine close-to-zero coordinates. */
#define EPSILON 0.01

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

      /* Prevent going out of the rendering space. */

      if (x1 < EPSILON)
        x1 = EPSILON;
      else if (x1 > world->width - 1 - EPSILON)
        x1 = world->width - 1 - EPSILON;

      if (y1 < EPSILON)
        {
          /* The bottom of the world is still a floor. */
          node->is_on_floor = 1;
          y1 = EPSILON;
        }
      else if (y1 > world->height - 1 - EPSILON)
        y1 = world->height - 1 - EPSILON;

      node->x = x1;
      node->y = y1;

      node->ix = round(node->x);
      node->iy = round(node->y);

      node = node->next;
    }
}
