#include <conio.h>

#include "world.h"

#define FPS 24

struct camera
{
  int x, y;
};

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
  struct world world = generate_world(512, 48);
  struct camera camera = {0, 0};

  for (;;)
    {
      switch (get_scancode())
        {
        case 0:
          break;
        case 30: /* A */
        case 75: /* left */
          camera.x--;
          break;
        case 32: /* D */
        case 77: /* right */
          camera.x++;
          break;
        case 17: /* W */
        case 72: /* up */
          camera.y--;
          break;
        case 31: /* S */
        case 80: /* down */
          camera.y++;
          break;
        case 1:  /* ESC */
          return 0;
        }

      print_world(&world, camera.x, camera.y);
      Sleep(1000 / FPS);
    }

  return 0;
}
