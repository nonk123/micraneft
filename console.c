#include "console.h"

#include <windows.h>

HANDLE get_stdout()
{
  return GetStdHandle(STD_OUTPUT_HANDLE);
}

HANDLE get_stdin()
{
  return GetStdHandle(STD_INPUT_HANDLE);
}

void get_console_window_size(int* rows, int* cols)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(get_stdout(), &csbi);

  /* Not sure what's happening in the RHS. */
  *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#define KEY_EVENTS_BUFFER 10

key_event_t* receive_key_events()
{
  INPUT_RECORD records[KEY_EVENTS_BUFFER];

  DWORD available;

  key_event_t empty = {1, 0, 0};
  key_event_t* events = malloc(sizeof(key_event_t));

  int i, index;

  events[0] = empty;

  GetNumberOfConsoleInputEvents(get_stdin(), &available);

  if (!available)
    return events;

  if (!ReadConsoleInput(get_stdin(), records, KEY_EVENTS_BUFFER, &available))
    return events;

  events = realloc(events, (available + 1) * sizeof(key_event_t));

  for (index = -1, i = 0; i < available; i++)
    {
      if (records[i].EventType == KEY_EVENT)
        index++;
      else
        continue;

      KEY_EVENT_RECORD record = records[i].Event.KeyEvent;
      key_event_t event;

      event.empty = 0;
      event.down = record.bKeyDown;
      event.scancode = record.wVirtualScanCode;

      events[index] = event;
    }

  events[index + 1] = empty;

  return events;
}

void set_color(unsigned char color)
{
  static unsigned char last_color = 0;

  if (last_color != color)
    {
      SetConsoleTextAttribute(get_stdout(), color);
      last_color = color;
    }
}

void disable_cursor()
{
  CONSOLE_CURSOR_INFO info;

  info.dwSize = 100;
  info.bVisible = FALSE;

  SetConsoleCursorInfo(get_stdout(), &info);
}

void move_cursor_to(int x, int y)
{
  COORD coord;

  coord.X = x;
  coord.Y = y;

  SetConsoleCursorPosition(get_stdout(), coord);
}
