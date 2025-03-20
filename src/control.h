#ifndef CONTROL_H
#define CONTROL_H

#include <ncurses.h>
#include <vector>

class Renderer;

class Button
{
    friend class Renderer;
    friend class Controller;

private:
    WINDOW *window;
    int line;
    int col;
    int height;
    int width;
    std::string label;

public:
    Button(WINDOW *win, int l, int c, int h, int w, const std::string &label);
    void set_label(const std::string &label);
    bool in_button(int y, int x);
};

class Controller
{
    friend class Renderer;

private:
    Renderer &renderer;
    WINDOW *map_window;
    WINDOW *operation_window;
    MEVENT event;
    std::vector<Button> buttons;

public:
    Controller(Renderer &r, std::vector<Button> &vb);
    void handle_key(short key);
};

#endif
