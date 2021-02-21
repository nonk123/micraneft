#include <conio.h>
#include <math.h>

#include "world.h"

#define FPS 20

void disable_cursor()
{
  CONSOLE_CURSOR_INFO info;

  info.dwSize = 100;
  info.bVisible = FALSE;

  SetConsoleCursorInfo(get_stdout(), &info);
}

/* If no key is pressed, return 0. Return its scancode otherwise. */
char get_scancode()
{
  INPUT_RECORD record;
  KEY_EVENT_RECORD event;

  DWORD available;

  GetNumberOfConsoleInputEvents(get_stdin(), &available);

  if (available == 0)
    return 0;

  if (!ReadConsoleInput(get_stdin(), &record, 1, &available))
    return 0;

  if (record.EventType != KEY_EVENT)
    return 0;

  event = record.Event.KeyEvent;

  if (event.bKeyDown)
    return event.wVirtualScanCode;
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
      disable_cursor();

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
