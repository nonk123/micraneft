#ifndef CONSOLE_H
#define CONSOLE_H

void get_console_window_size(int*, int*);

/* If no key is pressed, return 0. Return its scancode otherwise. */
int get_scancode();

void disable_cursor();

void move_cursor_to(int, int);

#endif /* CONSOLE_H */
