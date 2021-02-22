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

int get_scancode()
{
  INPUT_RECORD record;
  KEY_EVENT_RECORD event;

  DWORD available;

  GetNumberOfConsoleInputEvents(get_stdin(), &available);

  if (available == 0)
    return 0;

  if (!ReadConsoleInput(get_stdin(), &record, 1, &available))
    return 0;

  if (record.EventType != KEY_EVENT)
    return 0;

  event = record.Event.KeyEvent;

  if (event.bKeyDown)
    return event.wVirtualScanCode;
  else
    return 0;
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
