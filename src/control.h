#ifndef CONTROL_H
#define CONTROL_H

#include <ncurses.h>
#include <vector>

class Game;

class Controller
{
private:
    Game &game;

public:
    Controller(Game &g);
    void handle(short key);
};

#endif
