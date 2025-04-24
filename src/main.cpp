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

        short key;
        Loader loader = Loader();
        Menu start_menu = Menu("MISSILE COMMANDER", {"ENTER: START", "Q: QUIT"}, 2);
        Menu pause_menu = Menu("PAUSED", {"ESC: RESUME", "Q: QUIT"}, 2);
        // Menu end_menu = Menu("GAME OVER", {"ENTER: RESTART", "Q: QUIT"}, 2);
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());

        MenuRenderer start_menu_renderer = MenuRenderer(start_menu);
        MenuRenderer pause_menu_renderer = MenuRenderer(pause_menu);
        StartMenuController start_menu_controller = StartMenuController(start_menu);
        PauseMenuController pause_menu_controller = PauseMenuController(pause_menu);
        GameRenderer game_renderer = GameRenderer(game);
        GameController game_controller = GameController(game, pause_menu);

        // TODO: add FRAME_INTERVAL macro instead of magic number

        start_menu.activate();
        start_menu_renderer.init();
        while (start_menu.is_activated())
        {
            key = getch();
            start_menu_controller.handle(key);
            start_menu_renderer.draw();
            start_menu_renderer.render();
            usleep(10000);
        }

        game.activate();
        game_renderer.init();
        while (game.is_activated())
        {
            key = getch();
            game_controller.handle(key);
            if (!pause_menu.is_activated())
            {
                game_renderer.draw();
                game_renderer.render();
                usleep(10000);
                continue;
            }
            pause_menu_renderer.init();
            while (pause_menu.is_activated())
            {
                key = getch();
                pause_menu_controller.handle(key);
                pause_menu_renderer.draw();
                pause_menu_renderer.render();
                usleep(10000);
            }
            game_renderer.init();
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
