#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>
#include <string>
#include <vector>

class Controller;
class Button;

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

    WINDOW *get_operation_window(void) { return operation_window; };
    WINDOW *get_map_window(void) { return map_window; };

    void render(void);
    void draw_map(void);
    void draw_radar(int count);
    void draw_operation(std::vector<Button> &buttons);

    void draw_button(const Button &button);

    void draw_debug(const std::string &str);
};

#endif