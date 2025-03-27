#include <cstdlib>
#include <ncurses.h>
#include "game.h"
#include "control.h"

Controller::Controller(Game &g) : game(g)
{
    nodelay(stdscr, true);
    keypad(stdscr, true);
}

void Controller::handle_key(short key)
{
    switch (key)
    {
    case 'w':
        game.move_cursor(-1, 0);
        break;
    case 's':
        game.move_cursor(1, 0);
        break;
    case 'a':
        game.move_cursor(0, -1);
        break;
    case 'd':
        game.move_cursor(0, 1);
        break;

    case 'q':
        endwin();
        exit(0);
    }
}
