#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <ncurses.h>

class Menu;
class Game;

class Renderer
{
public:
    virtual void render(void) = 0;
    virtual void init(void) = 0;
    virtual void draw(void) = 0;
    void debug(const std::string &str, int line = 0);
};

class BasicMenuRenderer : public Renderer
{
private:
    Menu &menu;
    WINDOW *box_window;
    WINDOW *item_window;

public:
    BasicMenuRenderer(Menu &m) : menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

class TechMenuRenderer : public Renderer
{
private:
    TechMenu &menu;
    WINDOW *box_window;
    WINDOW *item_window;
    WINDOW *desc_window;

public:
    TechMenuRenderer(TechMenu &m) : menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

class GameRenderer : public Renderer
{
private:
    Game &game;
    WINDOW *box_window;
    WINDOW *map_window;
    WINDOW *info_window;
    WINDOW *operation_window;

public:
    GameRenderer(Game &g) : game(g) {};

    void init(void);
    void render(void);
    void draw(void);
};

#endif
