#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include <string>
#include <vector>

class Game;

class Renderer
{
public:
    virtual void render(void) = 0;
    virtual void init(void) = 0;
    virtual void draw(void) = 0;
    void debug(const std::string &str);
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

    void render(void);
    void init(void);
    void draw(void);
};

#endif
