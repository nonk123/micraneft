#include <stdio.h>

#include "world.h"

int main()
{
  struct world world = generate_world(32, 16);
  print_world(&world);
  return 0;
}
