#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "game.h"
#include "render.h"
#include "control.h"
#include "loader.h"

void init(void)
{
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    curs_set(0);
    start_color();
    nodelay(stdscr, true);
    keypad(stdscr, true);
}

int main(void)
{
    try
    {
        init();

        Loader loader = Loader();
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());

        GameRenderer gr = GameRenderer(game);
        GameController gc = GameController(game);

        Renderer &renderer = gr;
        Controller &controller = gc;

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
