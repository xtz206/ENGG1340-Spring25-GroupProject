#include <ncurses.h>
#include <iostream>

#include "render.h"

int main(void)
{
    try
    {
        // ncurses initialization
        setlocale(LC_CTYPE, "");
        initscr();
        curs_set(0);

        Renderer renderer;
        renderer.draw_map();
        renderer.render();

        refresh();
        getch();

        endwin();
    }
    catch (const std::exception &e)
    {
        endwin();
        std::cerr << e.what() << '\n';
    }

    return 0;
}
