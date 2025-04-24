#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "game.h"
#include "menu.h"
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
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

int main(void)
{
    try
    {
        init();

        Loader loader = Loader();
        Menu menu = Menu();
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());

        MenuRenderer menu_renderer = MenuRenderer(menu);
        MenuController menu_controller = MenuController(menu);
        GameRenderer game_renderer = GameRenderer(game);
        GameController game_controller = GameController(game);


        menu.activate();
        menu_renderer.init();
        while (menu.is_activated())
        {
            short key = getch();
            menu_controller.handle(key);
            menu_renderer.draw();
            menu_renderer.render();
            usleep(10000);
        }

        game.activate();
        game_renderer.init();
        while (game.is_activated())
        {
            short key = getch();
            game_controller.handle(key);
            game_renderer.draw();
            game_renderer.render();
            usleep(10000);
        }

        endwin();
        exit(0);
    }
    catch (const std::exception &e)
    {
        endwin();
        std::cerr << e.what() << '\n';
        exit(1);
    }

    return 0;
}
