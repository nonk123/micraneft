#include "world.h"

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

struct world generate_world(int width, int height)
{
  struct tile* buffer = calloc(width * height, sizeof(struct tile));

  struct tile empty = {' '};
  struct tile block = {'#'};

  struct world world = {buffer, width, height};

  int x, y;

  for (x = 0; x < width; x++)
    {
      for (y = 0; y < height / 2; y++)
        *get_tile(&world, x, y) = empty;

      for (; y < height; y++)
        *get_tile(&world, x, y) = block;
    }

  return world;
}

void print_world(struct world* world, int x0, int y0)
{
  int x, y;
  int tile_x, tile_y;

  int rows, cols;

  COORD coord;

  coord.X = 0;
  coord.Y = 0;

  /* Overwrite everything. */
  SetConsoleCursorPosition(get_stdout(), coord);

  get_console_window_size(&rows, &cols);

  for (y = 0; y < rows - 1; y++)
    {
      tile_y = y0 + y;

      for (x = 0; x < cols; x++)
        {
          tile_x = x0 + x;

          if (tile_y >= 0 && tile_x >= 0
              && tile_y < world->height && tile_x < world->width)
            _putch(get_tile(world, tile_x, tile_y)->displayed_as);
          else
            _putch(' ');
        }
    }
}

struct tile* get_tile(struct world* world, int x, int y)
{
  int index = world->height * x + y;
  return &world->tiles[index];
}

/* Windows-specific functions (aka trash). */

HANDLE get_stdout()
{
  static HANDLE stdout_handle = NULL;

  if (stdout_handle == NULL)
    stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

  return stdout_handle;
}

void get_console_window_size(int* rows, int* cols)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(get_stdout(), &csbi);

  /* Not sure what's happening in the RHS. */
  *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
