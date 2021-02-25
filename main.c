#include "conge/conge.h"

#include "world.h"

#define HOTBAR_SIZE 9

static struct
{
  conge_bool up, down, left, right, jump;
  conge_bool breaking, placing;
  tile_t hotbar[HOTBAR_SIZE];
  int hotbar_selection;
  int cursor_x, cursor_y; /* centered around the player */
} controls;

/* Hotbar bounds set at the start of the frame. */
static struct { int x0, y0, x1, y1; } hotbar_rect;

static entity_t* player = NULL;

/*
 * Run collision detection on ENTITY.
 *
 * X1 and Y1 point to its final coordinates. They are adjusted in the process.
 */
void
collide (conge_ctx* ctx, entity_t* entity, double* x1, double* y1)
{
  int x, y;

  for (x = 0; x < entity->width; x++)
    {
      int x0 = entity->ix + x;

      tile_t* above = get_tile (x0, entity->iy + entity->height);
      tile_t* below = get_tile (x0, entity->iy - 1);

      if ((is_impassable (*above, entity) && entity->vy > 0.0)
          || (is_impassable (*below, entity) && entity->vy < 0.0))
        {
          if (entity->vy < 0.0)
            entity->is_on_floor = 1;

          entity->vy = 0.0;
          *y1 = entity->y;
          break;
        }
    }

  /* Traverse from top to bottom to make the check below work. */
  for (y = entity->height - 1; y >= 0; y--)
    {
      int y0 = entity->iy + y;

      tile_t* left = get_tile (entity->ix - 1, y0);
      tile_t* right = get_tile (entity->ix + entity->width, y0);

      if ((is_impassable (*left, entity) && entity->vx < 0.0)
          || (is_impassable (*right, entity) && entity->vx > 0.0))
        {
          tile_t* above = get_tile (entity->ix, entity->iy + entity->height);

          /* Move up one tile if there's enough space. */
          if (y == 0 && !is_impassable (*above, entity))
            *y1 += 1;
          else
            {
              entity->vx = 0.0;
              *x1 = entity->x;
            }

          break;
        }
    }

  /* Check if we are on top of any climbable tiles. */
  for (x = 0; x < entity->width; x++)
    {
      tile_t* tile = get_tile (entity->ix + x, entity->iy);
      tile_t* below = get_tile (entity->ix + x, entity->iy - 1);

      int climbable_below = below->properties & CLIMBABLE;
      int climbable_at = tile->properties & CLIMBABLE;

      if (climbable_below || climbable_at)
        {
          entity->vy = 0.0;
          *y1 = entity->y;

          /* Prevent going under the ground from a ladder. */
          if (is_impassable (*below, entity) && entity->climb < 0.0)
            break;

          /* Prevent climbing up when standing on a ladder. */
          if (climbable_below && !climbable_at && entity->climb > 0.0)
            break;

          *y1 += entity->climb * ctx->timestep;
          break;
        }
    }
}

void
run_physics (conge_ctx* ctx)
{
  const double gravity = -13.4;
  const double epsilon = 0.01;

  entity_t* node = get_entities ();

  while (node)
    {
      double x1, y1;

      /* Acceleration is m * s^(-2) */
      node->vy += gravity * ctx->timestep;

      /* Apply movement. */
      x1 = node->x + node->vx * ctx->timestep;
      y1 = node->y + node->vy * ctx->timestep;

      /* Run collision detection. */
      node->is_on_floor = 0;
      collide (ctx, node, &x1, &y1);

      /* Prevent going out of the rendering space. */

      if (x1 < epsilon)
        x1 = epsilon;
      else if (x1 > WORLD_WIDTH - 1 - epsilon)
        x1 = WORLD_WIDTH - 1 - epsilon;

      if (y1 < epsilon)
        {
          /* The bottom of the world is still a floor. */
          node->is_on_floor = 1;
          y1 = epsilon;
        }
      else if (y1 > WORLD_HEIGHT - 1 - epsilon)
        y1 = WORLD_HEIGHT - 1 - epsilon;

      /* Finalize the changes. */

      node->x = x1;
      node->y = y1;

      node->ix = round (node->x);
      node->iy = round (node->y);

      node = node->next;
    }
}

void
render_world (conge_ctx* ctx)
{
  int x, y;

  entity_t* node = get_entities ();

  for (y = 0; y < ctx->rows; y++)
    for (x = 0; x < ctx->cols; x++)
      {
        conge_pixel* pixel = conge_get_pixel (ctx, x, y);
        tile_t* tile = NULL;

        int world_x = player->ix - ctx->cols / 2 + x;
        int world_y = player->iy - ctx->rows / 2 + (ctx->rows - 1 - y);

        if (world_x >= 0 && world_y >= 0 && world_x < WORLD_WIDTH && world_y < WORLD_HEIGHT)
          tile = get_tile (world_x, world_y);
        else
          tile = &empty_tile;

        pixel->character = tile->character;
        pixel->fg = tile->fg;
        pixel->bg = tile->bg;
      }

  /* Place entities on top of the frame. */
  while (node != NULL)
    {
      for (y = 0; y < node->height; y++)
        for (x = 0; x < node->width; x++)
          {
            int screen_x = ctx->cols / 2 + node->ix - player->ix + x;
            int screen_y = node->iy - player->iy + (ctx->rows - 1 - y) - ctx->rows / 2;

            if (screen_x >= 0 && screen_y >= 0 && screen_x < ctx->cols && screen_y < ctx->rows)
              {
                conge_pixel* pixel = conge_get_pixel (ctx, screen_x, screen_y);
                tile_t* part = get_part (node, x, y);

                pixel->character = part->character;
                pixel->fg = part->fg;
                pixel->bg = part->bg;
              }
          }

      node = node->next;
    }
}

void
render_ui (conge_ctx* ctx)
{
  int x, i;

  char fps_text[64], size_text[64];

  conge_pixel panel = {' ', CONGE_WHITE, CONGE_BLACK};

  /* Cursor's screen position. */
  int cursor_sx = ctx->cols / 2 + controls.cursor_x;
  int cursor_sy = ctx->rows / 2 - controls.cursor_y;

  /* Cursor's world position. */
  int cursor_wx = player->ix + controls.cursor_x;
  int cursor_wy = player->iy + controls.cursor_y;

  tile_t hotbar_item = controls.hotbar[controls.hotbar_selection];

  int in_range = is_cursor_in_range (controls.cursor_x, controls.cursor_y);
  int can_place = can_place_tile_at (cursor_wx, cursor_wy, hotbar_item, player);

  conge_pixel* cursor = conge_get_pixel (ctx, cursor_sx, cursor_sy);
  cursor->character = 'X';
  cursor->fg = in_range && can_place ? CONGE_WHITE : CONGE_RED;

  sprintf (fps_text, "FPS: %d", ctx->fps);
  conge_write_string (ctx, fps_text, 0, 0, CONGE_BLACK, CONGE_WHITE);

  sprintf (size_text, "%dx%d", ctx->cols, ctx->rows);
  conge_write_string (ctx, size_text, ctx->cols - strlen (size_text), 0, CONGE_BLACK, CONGE_WHITE);

  /* Hotbar. */

  for (i = 0, x = hotbar_rect.x0; x <= hotbar_rect.x1; x++, i++)
    {
      conge_pixel* pixel = conge_get_pixel (ctx, x, hotbar_rect.y0);
      conge_pixel* below = conge_get_pixel (ctx, x, hotbar_rect.y1);

      if (i % 2 && i / 2 < HOTBAR_SIZE)
        {
          int index = i / 2;

          tile_t item = controls.hotbar[index];

          pixel->character = item.character;
          pixel->fg = item.fg;
          pixel->bg = item.bg;

          *below = panel;
          below->character = '1' + index;

          if (index == controls.hotbar_selection)
            {
              below->fg = panel.bg;
              below->bg = panel.fg;
            }
        }
      else
        *pixel = *below = panel;
    }
}

void
accept_input (conge_ctx* ctx)
{
  int numeric;

  if (ctx->keys[CONGE_ESC])
    {
      ctx->exit = 1;
      return;
    }

  controls.up = ctx->keys[CONGE_W];
  controls.down = ctx->keys[CONGE_S];
  controls.left = ctx->keys[CONGE_A];
  controls.right = ctx->keys[CONGE_D];
  controls.jump = ctx->keys[CONGE_SPACEBAR];

  for (numeric = CONGE_1; numeric <= CONGE_9; numeric++)
    if (ctx->keys[numeric])
      controls.hotbar_selection = numeric - CONGE_1;

  controls.cursor_x = ctx->mouse_x - ctx->cols / 2;
  controls.cursor_y = (ctx->rows - 1 - ctx->mouse_y) - ctx->rows / 2;

  controls.placing = ctx->buttons & CONGE_LMB;
  controls.breaking = ctx->buttons & CONGE_RMB;

  /* Detect clicks on the hotbar. */
  if (ctx->mouse_y >= hotbar_rect.y0 && ctx->mouse_y <= hotbar_rect.y1
      && ctx->mouse_x >= hotbar_rect.x0
      && ctx->mouse_x <= hotbar_rect.x1
      && (controls.placing || controls.breaking))
    controls.hotbar_selection = (ctx->mouse_x - hotbar_rect.x0) / 2;

  controls.hotbar_selection -= ctx->scroll;

  if (controls.hotbar_selection < 0)
    controls.hotbar_selection = 0;
  else if (controls.hotbar_selection >= HOTBAR_SIZE)
    controls.hotbar_selection = HOTBAR_SIZE - 1;
}

void
tick (conge_ctx* ctx)
{
  const double walking_speed = 8.0;
  const double climbing_speed = 4.0;
  const double jump_power = 10.0;

  strcpy (ctx->title, "micraneft");

  hotbar_rect.x0 = ctx->cols / 2 - HOTBAR_SIZE;
  hotbar_rect.x1 = hotbar_rect.x0 + 2 * HOTBAR_SIZE;
  hotbar_rect.y0 = ctx->rows - 4;
  hotbar_rect.y1 = ctx->rows - 3;

  accept_input (ctx);

  if (controls.jump && player->is_on_floor)
    player->vy = jump_power;

  player->vx = controls.right * walking_speed;
  player->vx -= controls.left * walking_speed;

  player->climb = controls.up * climbing_speed;
  player->climb -= controls.down * climbing_speed;

  if (is_cursor_in_range (controls.cursor_x, controls.cursor_y))
    {
      tile_t tile = controls.hotbar[controls.hotbar_selection];

      int world_x = player->ix + controls.cursor_x;
      int world_y = player->iy + controls.cursor_y;

      if (controls.breaking && !controls.placing)
        remove_tile_at (world_x, world_y);
      else if (controls.placing && !controls.breaking)
        place_tile_at (world_x, world_y, tile, player);
    }

  run_physics (ctx);

  render_world (ctx);
  render_ui (ctx);
}

void
init_hotbar (void)
{
  tile_t wood = {' ', LIGHT_YELLOW, WHITE};
  tile_t diamond = {'*', stone_tile.bg, LIGHT_AQUA};
  tile_t glass = {'+', BACKGROUND, BLUE};

  controls.hotbar[0] = grass_tile;
  controls.hotbar[1] = dirt_tile;
  controls.hotbar[2] = stone_tile;
  controls.hotbar[3] = wood;
  controls.hotbar[4] = door_tile;
  controls.hotbar[5] = ladder_tile;
  controls.hotbar[6] = spike_tile;
  controls.hotbar[7] = diamond;
  controls.hotbar[8] = glass;
}

int
main (void)
{
  conge_ctx* ctx = conge_init ();

  generate_world ();
  player = get_entities ();

  init_hotbar ();

  conge_run (ctx, tick, 60);
  conge_free (ctx);

  return 0;
}
