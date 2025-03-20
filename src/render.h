#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include <string>

class Renderer
{
    friend class Controller;

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
    void draw_radar(int count);
    void draw_operation(void);

    void draw_debug(const std::string &str);
};

#endif