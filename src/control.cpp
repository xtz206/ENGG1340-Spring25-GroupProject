#include <cstdlib>
#include <ncurses.h>
#include "game.h"
#include "menu.h"
#include "control.h"

void MenuController::handle(short key)
{
    switch (key)
    {
    case '\n':
        menu.deactivate();
        return;

    case 'q':
        endwin();
        exit(0);
    }
}

void GameController::handle(short key)
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

    case '\n':
        game.pass_turn();
        return;

    case 'f':
        game.fix_city();
        return;

    case 'l':
        game.launch_cruise();
        return;

    case 'q':
        game.deactivate();
    }
}
