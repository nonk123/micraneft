#ifndef WORLD_H
#define WORLD_H

typedef struct tile_t {
  char displayed_as;
} tile_t;

typedef struct entity_t {
  struct entity_t* next;
  tile_t* parts;
  int ix, iy; /* properly rounded x and y */
  int width, height;
  double x, y, vx, vy;
  int is_on_floor;
} entity_t;

typedef struct world_t {
  tile_t* tiles;
  entity_t* entities;
  const int width, height, area;
} world_t;

world_t generate_world(int, int);

int is_opaque(tile_t *);

tile_t *get_tile(tile_t *, int, int, int, int);

tile_t *get_world_tile(world_t *, int, int);

tile_t *get_entity_tile(entity_t *, int, int);

/* Goes into print.c */
void print_world(world_t *, int, int);

/* Goes into tick.c */
void physics_tick(world_t *, double);

#endif /* WORLD_H */
