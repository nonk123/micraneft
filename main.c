#include <stdlib.h>
#include <math.h>
#include <sys\timeb.h>

#include <windows.h>

#include "print.h"
#include "world.h"
#include "console.h"

#define PHYSICS_FPS 30
#define MAX_FPS 60

#define MIN_DELTA (1.0 / MAX_FPS)
#define PHYSICS_DELTA (1.0 / PHYSICS_FPS)

typedef struct controls_t {
  int left, right, jump;
  int cursor_x, cursor_y; /* relative to the center of the screen */
  int breaking, placing;
} controls_t;

void process_input_event(controls_t* controls, input_event_t event)
{
  int rows, cols;

  get_console_window_size(&rows, &cols);

  if (event.type == TYPE_KEY_EVENT)
    {
      int down = event.key.down;

      switch (event.key.scancode)
        {
        case 30: /* A */
          controls->left = down;
          break;
        case 32: /* D */
          controls->right = down;
          break;
        case 57: /* Spacebar */
          controls->jump = down;
          break;
        case 1:  /* ESC */
          exit(0);
        }
    }
  else
    {
      controls->cursor_x = event.mouse.x - cols / 2;
      controls->cursor_y = (rows - 1 - event.mouse.y) - rows / 2;

      controls->placing = event.mouse.button & FROM_LEFT_1ST_BUTTON_PRESSED;
      controls->breaking = event.mouse.button & RIGHTMOST_BUTTON_PRESSED;
    }
}

void accept_input(controls_t* controls, entity_t* player, world_t* world)
{
  int i = 0;

  input_event_t* events = receive_input_events();
  input_event_t event = events[i++];

  const double walking_speed = 12.0;
  const double jump_power = 8.0;

  while (event.type != TYPE_END)
    {
      process_input_event(controls, event);
      event = events[i++];
    }

  free(events);

  if (controls->jump && player->is_on_floor)
    player->vy = jump_power;

  player->vx = controls->right * walking_speed;
  player->vx -= controls->left * walking_speed;

  if (cursor_in_range(controls->cursor_x, controls->cursor_y))
    {
      tile_t birch = {' ', LIGHT_YELLOW, WHITE};

      int world_x = player->ix + controls->cursor_x;
      int world_y = player->iy + controls->cursor_y;

      if (controls->breaking && !controls->placing)
        remove_tile_at(world, world_x, world_y);
      else if (controls->placing && !controls->breaking)
        place_tile_at(world, world_x, world_y, birch);
    }
}

int main()
{
  world_t world = generate_world();
  entity_t* player = world.entities;

  controls_t controls = {0};

  double last_frame = 0.0, current_frame;
  double second = 0.0;

  int actual_fps = 0, current_fps = 0;

  controls.cursor_x = 0.0;
  controls.cursor_y = -1.0;

  prepare_console();

  for (;;)
    {
      struct timeb start, end;

      frame_t frame;

      frame.world = &world;
      frame.fps = current_fps;

      ftime(&start);

      disable_cursor();

      accept_input(&controls, player, &world);
      physics_tick(&world, PHYSICS_DELTA);

      frame.cursor_x = controls.cursor_x;
      frame.cursor_y = controls.cursor_y;

      frame.player = player;

      print_frame(frame);

      ftime(&end);

      /* FPS calculations. */

      current_frame = (end.time - start.time) + 0.001 * (end.millitm - start.millitm);

      if (current_frame < MIN_DELTA)
        {
          /* Sleep to reach the desired FPS. */
          Sleep(1000 * (MIN_DELTA - current_frame));
          second += MIN_DELTA;
        }
      else
        second += current_frame;

      actual_fps++;

      if (second >= 1.0)
        {
          second -= 1.0;
          current_fps = actual_fps;
          actual_fps = 0;
        }

      last_frame = current_frame;
    }

  return 0;
}
