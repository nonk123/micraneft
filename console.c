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

void prepare_console()
{
  SetConsoleMode(get_stdin(), ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
}

void get_console_window_size(int* rows, int* cols)
{
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(get_stdout(), &csbi);

  /* Not sure what's happening in the RHS. */
  *cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  *rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

#define INPUT_EVENTS_BUFFER 10

input_event_t* receive_input_events()
{
  INPUT_RECORD records[INPUT_EVENTS_BUFFER];

  DWORD available;

  input_event_t end = {TYPE_END};
  input_event_t* events = malloc(sizeof(input_event_t));

  int i;

  events[0] = end;

  GetNumberOfConsoleInputEvents(get_stdin(), &available);

  if (!available)
    return events;

  if (!ReadConsoleInput(get_stdin(), records, INPUT_EVENTS_BUFFER, &available))
    return events;

  events = realloc(events, (available + 1) * sizeof(input_event_t));

  for (i = 0; i < available; i++)
    {
      input_event_t event;

      if (records[i].EventType == KEY_EVENT)
        {
          KEY_EVENT_RECORD record = records[i].Event.KeyEvent;

          event.type = TYPE_KEY_EVENT;
          event.key.down = record.bKeyDown;
          event.key.scancode = record.wVirtualScanCode;
        }
      else if (records[i].EventType == MOUSE_EVENT)
        {
          MOUSE_EVENT_RECORD record = records[i].Event.MouseEvent;

          event.type = TYPE_MOUSE_EVENT;
          event.mouse.button = record.dwButtonState;
          event.mouse.x = record.dwMousePosition.X;
          event.mouse.y = record.dwMousePosition.Y;
        }

      events[i] = event;
    }

  events[i] = end;

  return events;
}

void set_color(int color)
{
  static int last_color = 0;

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
