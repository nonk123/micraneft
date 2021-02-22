#include <stdlib.h>
#include <math.h>

#include <windows.h>

#include "world.h"
#include "console.h"

#define FPS 16

typedef struct controls_t {
  int left, right, up;
} controls_t;

void accept_input(controls_t* controls)
{
  int i = 0;

  key_event_t* events = receive_key_events();
  key_event_t event = events[i++];

  while (!event.empty)
    {
      switch (event.scancode)
        {
        case 30: /* A */
        case 75: /* left */
          controls->left = event.down;
          break;
        case 32: /* D */
        case 77: /* right */
          controls->right = event.down;
          break;
        case 17: /* W */
        case 72: /* up */
          controls->up = event.down;
          break;
        case 1:  /* ESC */
          exit(0);
        }

      event = events[i++];
    }

  free(events);
}

int main()
{
  world_t world = generate_world(1024, 64);
  entity_t* player = world.entities;

  controls_t controls = {0};

  const double walking_speed = 12.0;
  const double jump_power = 8.0;

  for (;;)
    {
      disable_cursor();

      accept_input(&controls);

      if (controls.up && player->is_on_floor)
        player->vy = jump_power;

      player->vx = controls.right * walking_speed;
      player->vx -= controls.left * walking_speed;

      physics_tick(&world, 1.0 / FPS);
      print_world(&world);

      Sleep(1000 / FPS);
    }

  return 0;
}
