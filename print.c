#include "print.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "world.h"
#include "console.h"

tile_t* get_buffer_tile(tile_t* buffer, int x, int y, int rows)
{
  return &buffer[rows * x + y];
}

tile_t* get_frame_tile(frame_t* frame, int x, int y)
{
  return get_buffer_tile(frame->buffer, x, y, frame->rows);
}

void project_world(frame_t* frame)
{
  int x, y;

  int x0 = frame->player->ix - frame->cols / 2;
  int y0 = frame->player->iy - frame->rows / 2;

  entity_t* node = frame->world->entities;

  /* Project the tiles. */
  for (x = 0; x < frame->cols; x++)
    for (y = 0; y < frame->rows; y++)
      {
        tile_t* tile = get_frame_tile(frame, x, y);

        int tile_x = x0 + x;
        int tile_y = y0 + y;

        if (tile_x >= 0 && tile_y >= 0 && tile_x < WORLD_WIDTH && tile_y < WORLD_HEIGHT)
          *tile = *get_world_tile(frame->world, tile_x, tile_y);
        else
          *tile = empty_tile;
      }

  /* Project the entities. */
  while (node)
    {
      for (y = 0; y < node->height; y++)
        {
          int screen_y = node->iy + y - y0;

          for (x = 0; x < node->width; x++)
            {
              int screen_x = node->ix + x - x0;

              if (screen_x >= 0 && screen_y >= 0 && screen_x < frame->cols && screen_y < frame->rows)
                {
                  tile_t* tile = get_frame_tile(frame, screen_x, screen_y);
                  *tile = *get_entity_tile(node, x, y);

                  if (tile->bg == BLACK)
                    tile->bg = sky_tile.bg;
                }
            }
        }

      node = node->next;
    }
}

void project_cursor(frame_t* frame)
{
  int screen_x = frame->cols / 2 + frame->cursor_x;
  int screen_y = frame->rows / 2 + frame->cursor_y;

  if (screen_x >= 0 && screen_y >= 0 && screen_x < frame->cols && screen_y < frame->rows)
    {
      tile_t* that_tile = get_frame_tile(frame, screen_x, screen_y);

      int world_x = frame->player->ix + frame->cursor_x;
      int world_y = frame->player->iy + frame->cursor_y;

      tile_t* tile_at_cursor = get_world_tile(frame->world, world_x, world_y);

      int can_reach = cursor_in_range(frame->cursor_x, frame->cursor_y);
      int can_place = can_place_tile_at(frame->world, world_x, world_y);

      that_tile->character = 'X';
      that_tile->fg = can_place && can_reach ? LIGHT_YELLOW : LIGHT_RED;
    }
}

#define BUFFER 64

void project_fps(frame_t* frame)
{
  int i;
  char string[BUFFER];

  sprintf(string, "FPS: %d", frame->fps);

  for (i = 0; string[i] != '\0'; i++)
    {
      /* Strip of text in the top-left corner. */
      tile_t* tile = get_frame_tile(frame, i, frame->rows - 1);

      *tile = empty_tile;
      tile->bg = WHITE;
      tile->fg = BLACK;
      tile->character = string[i];
    }
}

void project_resolution(frame_t* frame)
{
  int i, len;
  char string[BUFFER];

  sprintf(string, "%dx%d", frame->cols, frame->rows);

  len = strlen(string);

  for (i = 0; i < len; i++)
    {
      /* Top-right corner. */
      tile_t* tile = get_frame_tile(frame, frame->cols - len + i, frame->rows - 1);

      tile->bg = WHITE;
      tile->fg = BLACK;
      tile->character = string[i];
    }
}

#undef BUFFER

void print_frame(frame_t frame)
{
  int x, y, i;

  static tile_t* backbuffer = NULL;
  frame_t* this_frame;

  static int old_buffer_size = 0;
  int buffer_size;

  /* Magic values to trigger the check below. */
  int cursor_x = -2, cursor_y = 0;

  get_console_window_size(&frame.rows, &frame.cols);

  buffer_size = frame.rows * frame.cols * sizeof(tile_t);

  frame.buffer = malloc(buffer_size);
  backbuffer = realloc(backbuffer, buffer_size);

  project_world(&frame);
  project_cursor(&frame);
  project_fps(&frame);
  project_resolution(&frame);

  /* Redraw when the window size changes. */
  if (buffer_size != old_buffer_size)
    memset(backbuffer, 0, buffer_size); /* fill with junk */

  old_buffer_size = buffer_size;

  for (y = 0; y < frame.rows; y++)
    for (x = 0; x < frame.cols; x++)
      {
        tile_t* draw = get_frame_tile(&frame, x, y);
        int equal = 0;

        if (backbuffer != NULL)
          {
            tile_t* behind = get_buffer_tile(backbuffer, x, y, frame.rows);
            equal = tiles_equal_p(draw, behind);
          }

        if (!equal)
          {
            /* putchar moves the cursor anyways. */
            if ((cursor_y != y || x - cursor_x > 1) || (cursor_y == y - 1 && x == 0))
                /* The entire world is upside-down. */
                move_cursor_to(x, frame.rows - 1 - y);

            set_color(16 * draw->bg + draw->fg);
            putchar(draw->character);

            cursor_x = x;
            cursor_y = y;
          }
      }

  memcpy(backbuffer, frame.buffer, buffer_size);
  free(frame.buffer);

  /* Restore formatting in case we'll exit. */
  move_cursor_to(0, 0);
  set_color(WHITE);
}
