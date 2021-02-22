#ifndef WORLD_H
#define WORLD_H

struct tile
{
  char displayed_as;
};

struct entity
{
  struct entity* next;
  struct tile* parts;
  int ix, iy; /* properly rounded x and y */
  int width, height;
  double x, y, vx, vy;
  int is_on_floor;
};

struct world
{
  struct tile* tiles;
  struct entity* entities;
  const int width, height, area;
};

struct world generate_world(int, int);

int is_opaque(struct tile *);

struct tile *get_tile(struct tile *, int, int, int, int);

struct tile *get_world_tile(struct world *, int, int);

struct tile *get_entity_tile(struct entity *, int, int);

/* Goes into print.c */
void print_world(struct world *);

/* Goes into tick.c */
void physics_tick(struct world *, double);

#endif /* WORLD_H */
