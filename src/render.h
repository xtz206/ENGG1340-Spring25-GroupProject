#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>

class Renderer
{
private:
    WINDOW *map_window;
    WINDOW *radar_window;
    WINDOW *node_window;
    WINDOW *info_window;
    WINDOW *operation_window;

public:
    Renderer(void);
    ~Renderer(void) {};
    void render(void);
    void draw_map(void);
};

#endif