#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>

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

/* Typedeffed to a "small" type to save memory. */
typedef unsigned char property;

typedef struct tile_t {
  unsigned char character;
  color_t bg, fg;
  property properties;
} tile_t;

enum properties {
  CLIMBABLE = 1, /* climb this tile with W/S */
  DOOR = 2, /* only the player can pass through this tile */
};

typedef struct entity_t {
  struct entity_t* next;
  int width, height;
  tile_t* parts;
  double x, y;
  double vx, vy; /* velocity vector */
  double climb; /* climbing vertical velocity */
  int ix, iy; /* properly rounded x and y */
  int is_player; /* has special effect on stuff */
  int is_on_floor;
} entity_t;

/* Pretty much the redefinition of conge's stuff. */
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

/* Common tiles. */

/* The default background color for (transparent) tiles. */
#define BACKGROUND BLACK

static tile_t empty_tile = {' ', BACKGROUND, WHITE};

static tile_t grass_tile = {' ', GREEN, WHITE};
static tile_t dirt_tile  = {' ', YELLOW, WHITE};
static tile_t stone_tile = {' ', WHITE, WHITE};

static tile_t door_tile = {'|', BACKGROUND, YELLOW, DOOR};
static tile_t ladder_tile = {'#', BACKGROUND, YELLOW, CLIMBABLE};
static tile_t spike_tile = {'^', BACKGROUND, WHITE}; /* TODO: make it sharp */

void generate_world();

entity_t* get_entities();

int are_tiles_equal(tile_t*, tile_t*);

int is_occupied(tile_t*);

int is_impassable(tile_t*, entity_t*);

int is_cursor_in_range(int, int);

int can_place_tile_at(int, int);

void place_tile_at(int, int, tile_t);

void remove_tile_at(int, int);

tile_t* get_tile(int, int);

tile_t* get_part(entity_t*, int, int);

#endif /* WORLD_H */
