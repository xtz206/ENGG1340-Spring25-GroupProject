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

class MenuRenderer : public Renderer
{
private:
    Menu &menu;
    WINDOW *menu_window;

public:
    MenuRenderer(Menu &m) : menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

class GameRenderer : public Renderer
{
private:
    Game &game;
    WINDOW *map_window;
    WINDOW *radar_window;
    WINDOW *node_window;
    WINDOW *info_window;
    WINDOW *operation_window;

public:
    GameRenderer(Game &g) : game(g) {};

    void init(void);
    void render(void);
    void draw(void);
};

#endif
