#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ncurses.h>
#include "render.h"
#include "control.h"

Button::Button(WINDOW *win, int l, int c, int h, int w, const std::string &label_)
    : window(win), line(l), col(c), height(h), width(w), label(label_) {};

void Button::set_label(const std::string &label_)
{
    this->label = label_;
}

bool Button::in_button(int y, int x)
{
    return y >= line && y < line + height && x >= col && x < col + width;
}

Controller::Controller(Renderer &r, std::vector<Button> &vb)
    : renderer(r), map_window(r.map_window), operation_window(r.operation_window), buttons(vb)
{
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
                std::ostringstream oss;
                std::stringstream msg;

                for (auto button : buttons)
                {
                    int win_y = event.y;
                    int win_x = event.x;
                    if (wmouse_trafo(button.window, &win_y, &win_x, false) && button.in_button(win_y, win_x))
                    {
                        msg << "LClick button " << button.label << " at (" << std::setw(3) << win_y << ", " << std::setw(3) << win_x << ")";
                        oss.setf(std::ios::left);
                        oss << std::setw(50) << msg.str();
                        oss.unsetf(std::ios::left);
                        renderer.draw_debug(oss.str());
                        return;
                    }
                }
            }
        }
        return;

    case 'q':
        throw std::runtime_error("Exit");
        return;
    }
}
