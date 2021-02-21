#include <conio.h>

#include "world.h"

#define FPS 8

int main()
{
  struct world world = generate_world(512, 30);

  for (;;)
    {
      if (_kbhit()) break;
      print_world(&world, 0, 0);
      Sleep(1000 / FPS);
    }

  return 0;
}
