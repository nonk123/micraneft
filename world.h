#ifndef WORLD_H
#define WORLD_H

#define WORLD_WIDTH 1024
#define WORLD_HEIGHT 128

#define GROUND_HEIGHT 64
#define HILLS_HEIGHT 32

/* The height of the grass layer. */
#define GRASS_LAYER 2

/* Place this many blocks of dirt below grass. Everything below this is stone. */
#define DIRT_LAYER 8

#define NOISE_PERIOD 64

typedef unsigned char color_t;

typedef struct tile_t {
  char character;
  color_t bg, fg;
} tile_t;

typedef struct entity_t {
  struct entity_t* next;
  tile_t* parts;
  int ix, iy; /* properly rounded x and y */
  int width, height;
  double x, y;
  double vx, vy; /* velocity vector */
  int is_on_floor;
} entity_t;

typedef struct world_t {
  tile_t* tiles;
  entity_t* entities;
} world_t;

enum text_color {
  BLACK,
  BLUE,
  GREEN,
  AQUA,
  RED,
  PURPLE,
  YELLOW,
  WHITE,
  GRAY,
  LIGHT_BLUE,
  LIGHT_GREEN,
  LIGHT_AQUA,
  LIGHT_RED,
  LIGHT_PURPLE,
  LIGHT_YELLOW,
  BRIGHT_WHITE,
};

static tile_t empty_tile = {' ', BLACK, WHITE};
static tile_t sky_tile = {' ', LIGHT_AQUA, WHITE};

world_t generate_world();

int tiles_equal_p(tile_t *, tile_t *);

int tile_opaque_p(tile_t *);

int cursor_in_range(int, int);

int can_place_tile_at(world_t *, int, int);

void place_tile_at(world_t *, int, int, tile_t);

void remove_tile_at(world_t *, int, int);

tile_t *get_tile(tile_t *, int, int, int, int);

tile_t *get_world_tile(world_t *, int, int);

tile_t *get_entity_tile(entity_t *, int, int);

/* Goes into tick.c */
void physics_tick(world_t *, double);

#endif /* WORLD_H */
