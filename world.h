#ifndef WORLD_H
#define WORLD_H

typedef struct tile_t {
  char character;
  unsigned char bg, fg;
  char opaque;
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
  const int width, height, area;
} world_t;

enum text_color
{
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

static const tile_t empty_tile = {' ', BLACK, WHITE, 0};

world_t generate_world(int, int);

int tiles_equal_p(tile_t *, tile_t *);

tile_t *get_tile(tile_t *, int, int, int, int);

tile_t *get_world_tile(world_t *, int, int);

tile_t *get_entity_tile(entity_t *, int, int);

/* Goes into tick.c */
void physics_tick(world_t *, double);

#endif /* WORLD_H */
