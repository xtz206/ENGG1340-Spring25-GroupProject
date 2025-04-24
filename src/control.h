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
protected:
    Menu &menu;

public:
    MenuController(Menu &m) : menu(m) {};
};

class StartMenuController : public MenuController
{
public:
    StartMenuController(Menu &m) : MenuController(m) {};
    virtual void handle(short key) override;
};

class PauseMenuController : public MenuController
{
public:
    PauseMenuController(Menu &m) : MenuController(m) {};
    virtual void handle(short key) override;
};

class GameController : public Controller
{
private:
    Game &game;
    Menu &menu;

public:
    GameController(Game &g, Menu &m) : game(g), menu(m) {};
    virtual void handle(short key) override;
};

#endif
