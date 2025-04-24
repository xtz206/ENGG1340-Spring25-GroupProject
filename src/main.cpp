#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "game.h"
#include "render.h"
#include "control.h"
#include "loader.h"

int main(void)
{
    try
    {
        Loader loader = Loader();
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());
        Renderer renderer = Renderer(game);
        Controller controller = Controller(game);

        renderer.init();
        while (true)
        {

            short key = getch();
            controller.handle(key);

            renderer.draw();
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
