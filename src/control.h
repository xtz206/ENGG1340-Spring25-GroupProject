#ifndef CONTROL_H
#define CONTROL_H

#include <ncurses.h>
#include <vector>

class Game;

class Controller
{
public:
    virtual void handle(short key) = 0;
};

class GameController : public Controller
{
private:
    Game &game;

public:
    GameController(Game &g) : game(g) {};
    void handle(short key) override;
};

#endif
