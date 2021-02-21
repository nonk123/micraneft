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
  int rows, cols;
  int width, height;

  COORD coord;

  coord.X = 0;
  coord.Y = 0;

  /* Overwrite everything. */
  SetConsoleCursorPosition(get_stdout(), coord);

  get_console_window_size(&rows, &cols);

  width = min(cols, world->width);
  height = min(rows - 1, world->height);

  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        _putch(get_tile(world, x0 + x, y0 + y)->displayed_as);

      /* Wrap manually if the world is smaller than the console window. */
      if (width < cols && y < cols - 1)
        _putch('\n');
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
