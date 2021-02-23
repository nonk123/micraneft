#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys\timeb.h>

#include <windows.h>

#include "print.h"
#include "world.h"
#include "console.h"

#define PHYSICS_FPS 30
#define MAX_FPS 60

typedef struct controls_t {
  int left, right, up;
} controls_t;

void accept_input(controls_t* controls, entity_t* player)
{
  int i = 0;

  key_event_t* events = receive_key_events();
  key_event_t event = events[i++];

  const double walking_speed = 12.0;
  const double jump_power = 8.0;

  while (!event.empty)
    {
      int down = event.down;

      switch (event.scancode)
        {
        case 30: /* A */
        case 75: /* left */
          controls->left = down;
          break;
        case 32: /* D */
        case 77: /* right */
          controls->right = down;
          break;
        case 17: /* W */
        case 72: /* up */
          controls->up = down;
          break;
        case 1:  /* ESC */
          exit(0);
        }

      event = events[i++];
    }

  free(events);

  if (controls->up && player->is_on_floor)
    player->vy = jump_power;

  player->vx = controls->right * walking_speed;
  player->vx -= controls->left * walking_speed;
}

int main()
{
  world_t world = generate_world(1024, 64);
  entity_t* player = world.entities;

  controls_t controls = {0};

  const double min_delta = 1.0 / MAX_FPS;
  const double physics_delta = 1.0 / PHYSICS_FPS;

  double last_frame = 0.0, current_frame;
  double second = 0.0;

  int actual_fps = 0, current_fps = 0;

  for (;;)
    {
      struct timeb start, end;

      frame_t frame;

      frame.world = &world;
      frame.fps = current_fps;

      ftime(&start);

      disable_cursor();

      accept_input(&controls, player);
      physics_tick(&world, physics_delta);

      frame.center_x = player->ix;
      frame.center_y = player->iy + 1;

      print_frame(frame);

      ftime(&end);

      /* FPS calculations. */

      current_frame = (end.time - start.time) + 0.001 * (end.millitm - start.millitm);

      if (current_frame < min_delta)
        {
          /* Sleep to reach the desired FPS. */
          Sleep(1000 * (min_delta - current_frame));
          second += min_delta;
        }
      else
        second += current_frame;

      actual_fps++;

      if (second >= 1.0)
        {
          second -= 1.0;
          current_fps = actual_fps;
          actual_fps = 0;
        }

      last_frame = current_frame;
    }

  return 0;
}
