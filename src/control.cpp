#include <cstdlib>
#include <ncurses.h>
#include "control.h"

Controller::Controller()
{
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
