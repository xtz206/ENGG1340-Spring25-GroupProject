#include <cstdlib>
#include <ncurses.h>
#include "control.h"

Controller::Controller()
{
    nodelay(stdscr, true);
    keypad(stdscr, true);
}

void Controller::handle_key(short key)
{
    switch (key)
    {
    case 'q':
        endwin();
        exit(0);
    }
}
