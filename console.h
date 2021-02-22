#ifndef CONSOLE_H
#define CONSOLE_H

typedef struct key_event_t {
  int empty;
  int down, scancode;
} key_event_t;

void get_console_window_size(int*, int*);

key_event_t* receive_key_events();

void set_color(unsigned char);

void disable_cursor();

void move_cursor_to(int, int);

#endif /* CONSOLE_H */
