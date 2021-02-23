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

#define HOTBAR_SIZE 9

typedef struct controls_t {
  int left, right, jump;
  int cursor_x, cursor_y; /* relative to the center of the screen */
  int breaking, placing;
  tile_t hotbar[HOTBAR_SIZE];
  int hotbar_selection;
} controls_t;

void process_input_event(controls_t* controls, input_event_t event)
{
  int rows, cols;

  get_console_window_size(&rows, &cols);

  if (event.type == TYPE_KEY_EVENT)
    {
      int down = event.key.down;
      int numeric = event.key.scancode - 2;

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

      if (numeric >= 0 && numeric < HOTBAR_SIZE)
        controls->hotbar_selection = numeric;
    }
  else /* mouse event */
    {
      int hotbar_x0 = cols / 2 - HOTBAR_SIZE;
      int hotbar_x1 = hotbar_x0 + 2 * HOTBAR_SIZE + 1;
      int hotbar_y0 = 2, hotbar_y1 = 3;

      int mouse_y = rows - 1 - event.mouse.y;

      controls->cursor_x = event.mouse.x - cols / 2;
      controls->cursor_y = mouse_y - rows / 2;

      controls->placing = event.mouse.button & FROM_LEFT_1ST_BUTTON_PRESSED;
      controls->breaking = event.mouse.button & RIGHTMOST_BUTTON_PRESSED;

      /* Detect clicks on the hotbar. */
      if (mouse_y >= hotbar_y0 && mouse_y <= hotbar_y1
          && event.mouse.x >= hotbar_x0
          && event.mouse.x <= hotbar_x1
          && (controls->placing || controls->breaking))
        {
          int slot = (event.mouse.x - hotbar_x0) / 2;
          controls->hotbar_selection = slot;
          controls->placing = controls->breaking = 0;
        }

      if (event.mouse.event == MOUSE_WHEELED)
        {
          controls->hotbar_selection -= event.mouse.button / abs(event.mouse.button);

          if (controls->hotbar_selection < 0)
            controls->hotbar_selection = 0;
          else if (controls->hotbar_selection >= HOTBAR_SIZE)
            controls->hotbar_selection = HOTBAR_SIZE - 1;
        }
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
      tile_t tile = controls->hotbar[controls->hotbar_selection];

      int world_x = player->ix + controls->cursor_x;
      int world_y = player->iy + controls->cursor_y;

      if (controls->breaking && !controls->placing)
        remove_tile_at(world, world_x, world_y);
      else if (controls->placing && !controls->breaking)
        place_tile_at(world, world_x, world_y, tile);
    }
}

void init_hotbar(tile_t* hotbar)
{
  tile_t wood = {' ', LIGHT_YELLOW, WHITE};

  tile_t coal = {'*', stone_tile.bg, BLACK};
  tile_t iron = {'*', stone_tile.bg, BRIGHT_WHITE};
  tile_t gold = {'*', stone_tile.bg, YELLOW};
  tile_t diamond = {'*', stone_tile.bg, LIGHT_AQUA};

  tile_t glass = {'+', LIGHT_AQUA, BLUE};

  hotbar[0] = grass_tile;
  hotbar[1] = dirt_tile;
  hotbar[2] = stone_tile;
  hotbar[3] = wood;
  hotbar[4] = coal;
  hotbar[5] = iron;
  hotbar[6] = gold;
  hotbar[7] = diamond;
  hotbar[8] = glass;
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

  init_hotbar(controls.hotbar);
  prepare_console();

  for (;;)
    {
      struct timeb start, end;
      frame_t frame;

      ftime(&start);

      disable_cursor();

      accept_input(&controls, player, &world);
      physics_tick(&world, PHYSICS_DELTA);

      frame.world = &world;
      frame.fps = current_fps;

      frame.cursor_x = controls.cursor_x;
      frame.cursor_y = controls.cursor_y;

      frame.player = player;

      frame.hotbar = controls.hotbar;
      frame.hotbar_size = HOTBAR_SIZE;
      frame.hotbar_selection = controls.hotbar_selection;

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
