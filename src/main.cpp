#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "game.h"
#include "render.h"
#include "control.h"

int main(void)
{
    try
    {
        Game game;
        Renderer renderer = Renderer(game);
        Controller controller = Controller(game);

        renderer.draw_window();
        while (true)
        {

            short key = getch();
            controller.handle_key(key);

            renderer.draw_game();
            renderer.render();
            usleep(10000);
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
