#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include <string>
#include <vector>

class Game;

class Renderer
{
private:
    Game &game;
    WINDOW *map_window;
    WINDOW *radar_window;
    WINDOW *node_window;
    WINDOW *info_window;
    WINDOW *operation_window;

public:
    Renderer(Game &game);
    ~Renderer(void) {};

    void render(void);
    void draw_window(void);
    void draw_game(void);
    void draw_debug(const std::string &str);
};

#endif
