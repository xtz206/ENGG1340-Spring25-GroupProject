#ifndef CONTROL_H
#define CONTROL_H

#include <ncurses.h>
#include <vector>

class Menu;
class Game;

class Controller
{
public:
    virtual void handle(short key) = 0;
};

class MenuController : public Controller
{
private:
    Menu &menu;

public:
    MenuController(Menu &m) : menu(m) {};
    void handle(short key) override;
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
