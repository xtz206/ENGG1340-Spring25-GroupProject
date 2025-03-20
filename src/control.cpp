#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ncurses.h>
#include "render.h"
#include "control.h"

Controller::Controller(Renderer *r)
{
    this->renderer = r;
    this->map_window = r->map_window;
    this->operation_window = r->operation_window;
}

void Controller::handle_key(short key)
{
    switch (key)
    {
    case KEY_MOUSE:
        if (getmouse(&event) == OK)
        {

            if (event.bstate & BUTTON1_CLICKED)
            {
                if (wmouse_trafo(map_window, &event.y, &event.x, false))
                {
                    std::stringstream ss;
                    ss << std::setw(50) << "LClick at (" << std::setw(3) << event.y << ", " << std::setw(3) << event.x << ") in map window";
                    renderer->draw_debug(ss.str()); // 11 + 3 + 2 + 3 + 15 = 34
                }
                else if (wmouse_trafo(operation_window, &event.y, &event.x, false))
                {
                    std::stringstream ss;
                    if (event.y == 1)
                    {
                        ss << std::setw(50) << "LClick at Build Cruise";
                    }
                    else if (event.y == 2)
                    {
                        ss << std::setw(50) << "LClick at Build IRBM";
                    }
                    else if (event.y == 3)
                    {
                        ss << std::setw(50) << "LClick at Launch Cruise";
                    }
                    else if (event.y == 4)
                    {
                        ss << std::setw(50) << "LClick at Launch IRBM";
                    }
                    else if (event.y == 5)
                    {
                        ss << std::setw(50) << "LClick at Launch Fix";
                    }
                    else
                    {
                        ss << std::setw(50) << "LClick at (" << std::setw(3) << event.y << ", " << std::setw(3) << event.x << ") in operation window";
                    }
                    renderer->draw_debug(ss.str());
                }
            }
        }
        break;

    case 'q':
        throw std::runtime_error("Exit");
        break;
    }
}