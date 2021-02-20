#ifndef WORLD_H
#define WORLD_H

struct tile {
  char displayed_as;
};

struct world {
  struct tile* tiles;
  int width, height;
};

struct world generate_world(int, int);

void print_world(struct world*);

struct tile* get_tile(struct world*, int, int);

#endif
