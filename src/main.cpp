#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "render.h"
#include "control.h"

int main(void)
{
    try
    {
        // ncurses initialization
        setlocale(LC_CTYPE, "");
        initscr();
        noecho();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        mousemask(BUTTON1_CLICKED, NULL);
        curs_set(0);

        Renderer renderer = Renderer();
        Controller controller = Controller();

        int count = 1;

        renderer.draw_map();
        while (true)
        {

            short key = getch();
            controller.handle_key(key);

            renderer.render();
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
