#include <ncurses.h>
#include <iostream>
#include <unistd.h>

#include "render.h"

int main(void)
{
    try
    {
        // ncurses initialization
        setlocale(LC_CTYPE, "");
        initscr();
        noecho();
        nodelay(stdscr, true);
        curs_set(0);

        Renderer renderer;
        char ch;

        int count = 1;
        while (true)
        {
            renderer.draw_map();
            renderer.draw_operation();
            renderer.draw_radar(count);
            renderer.render();
            refresh();
            char ch = getch();
            if (ch == 'q')
            {
                break;
            }
            usleep(100000);
            count++;
        }

        endwin();
    }
    catch (const std::exception &e)
    {
        endwin();
        std::cerr << e.what() << '\n';
    }

    return 0;
}
