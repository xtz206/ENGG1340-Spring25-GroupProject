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
        game.move_cursor(Position(-1, 0));
        return;
    case 's':
        game.move_cursor(Position(1, 0));
        return;
    case 'a':
        game.move_cursor(Position(0, -1));
        return;
    case 'd':
        game.move_cursor(Position(0, 1));
        return;

    case 'c':
        game.pass_turn();
        return;

    case 'f':
        game.fix_city();
        return;

    case 'l':
        game.launch_cruise();
        return;

    case 'q':
        endwin();
        exit(0);
    }
}
