#ifndef CONTROL_H
#define CONTROL_H

#include <ncurses.h>
#include <vector>

class Controller
{

public:
    Controller(void);
    void handle_key(short key);
};

#endif
