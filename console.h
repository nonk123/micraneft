#ifndef CONSOLE_H
#define CONSOLE_H

enum input_event_type {
  TYPE_END,
  TYPE_KEY_EVENT,
  TYPE_MOUSE_EVENT,
};

typedef struct input_event_t {
  enum input_event_type type;
  union {
    struct {
      char down;
      int scancode;
    } key;
    struct {
      int x, y;
      int button;
      int event;
    } mouse;
  };
} input_event_t;

void prepare_console();

void get_console_window_size(int*, int*);

input_event_t* receive_input_events();

void set_color(int);

void disable_cursor();

void move_cursor_to(int, int);

#endif /* CONSOLE_H */
