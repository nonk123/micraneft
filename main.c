#include <conio.h>

#include "world.h"

#define FPS 12

struct camera
{
  int x, y;
};

int main()
{
  struct world world = generate_world(512, 48);
  struct camera camera = {0, 0};

  for (;;)
    {
      if (_kbhit())
        switch (_getch())
          {
          case 'a':
            camera.x--;
            break;
          case 'd':
            camera.x++;
            break;
          case 'w':
            camera.y--;
            break;
          case 's':
            camera.y++;
            break;
          case 'q':
            return 0;
          }

      print_world(&world, camera.x, camera.y);
      Sleep(1000 / FPS);
    }

  return 0;
}
