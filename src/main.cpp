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

        Renderer renderer = Renderer();
        Controller controller = Controller();

        renderer.draw_window();
        renderer.draw_map();
        while (true)
        {

            short key = getch();
            controller.handle_key(key);

            renderer.render();
            usleep(100000);
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
