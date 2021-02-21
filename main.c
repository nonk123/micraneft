#include <conio.h>
#include <math.h>

#include "world.h"

#define FPS 20

/* If no key is pressed, return 0. Return its scancode otherwise. */
char get_scancode()
{
  INPUT_RECORD records[1];
  KEY_EVENT_RECORD record;

  DWORD available;

  GetNumberOfConsoleInputEvents(get_stdin(), &available);

  if (available == 0)
    return 0;

  ReadConsoleInput(get_stdin(), records, 1, &available);

  if (records[0].EventType != KEY_EVENT)
    return 0;

  record = records[0].Event.KeyEvent;

  if (record.bKeyDown)
    return record.wVirtualScanCode;
  else
    return 0;
}

int main()
{
  struct world world = generate_world(128, 32);
  struct entity* player = world.entities;

  const double walking_speed = 12.0;
  const double jump_power = 8.0;

  for (;;)
    {
      switch (get_scancode())
        {
        case 0: /* no key */
          if (player->is_on_floor)
            player->vx = 0.0;
          break;
        case 30: /* A */
        case 75: /* left */
          player->vx = -walking_speed;
          break;
        case 32: /* D */
        case 77: /* right */
          player->vx = walking_speed;
          break;
        case 17: /* W */
        case 72: /* up */
          if (player->is_on_floor)
            player->vy = jump_power;
          break;
        case 1:  /* ESC */
          return 0;
        }

      physics_tick(&world, 1.0 / FPS);
      print_world(&world);

      Sleep(1000 / FPS);
    }

  return 0;
}
