#ifndef CONTROL_H
#define CONTROL_H

#include <ncurses.h>
#include "render.h"

class Controller
{
    friend class Renderer;

private:
    Renderer *renderer;
    WINDOW *map_window;
    WINDOW *operation_window;
    MEVENT event;

public:
    Controller(Renderer *renderer);
    void handle_key(short key);
};

#endif