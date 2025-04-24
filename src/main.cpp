#include <iostream>
#include <string>
#include <ncurses.h>
#include <unistd.h>

#include "game.h"
#include "menu.h"
#include "render.h"
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

void control(short key, Game &game, Menu &start_menu, Menu &pause_menu, Menu &end_menu)
{
    if (start_menu.is_activated())
    {
        switch (key)
        {
        case '\n':
            start_menu.deactivate();
            game.activate();
            return;
        case 'q':
            start_menu.deactivate();
            return;
        }
    }
    else if (game.is_activated())
    {
        switch (key)
        {
        case 'w':
            game.move_cursor(Position(-1, 0));
            return;
        case 's':
            game.move_cursor(Position(1, 0));
            return;
        case 'a':
            game.move_cursor(Position(0, -1));
            return;
        case 'd':
            game.move_cursor(Position(0, 1));
            return;

        case '\n':
            game.pass_turn();
            return;

        case '\033':
            game.deactivate();
            pause_menu.activate();
            return;

        case 'f':
            game.fix_city();
            return;

        case 'l':
            game.launch_cruise();
            return;

        case 'q':
            game.deactivate();
        }
    }
    else if (pause_menu.is_activated())
    {
        switch (key)
        {
        case '\033':
            pause_menu.deactivate();
            game.activate();
            return;
        case 'q':
            pause_menu.deactivate();
            return;
        }
    }
    else if (end_menu.is_activated())
    {
        switch (key)
        {
        case '\n':
            end_menu.deactivate();
            game.activate();
            return;
        case 'q':
            end_menu.deactivate();
            return;
        }
    }
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
        Menu end_menu = Menu("GAME OVER", {"ENTER: RESTART", "Q: QUIT"}, 2);
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());

        MenuRenderer start_menu_renderer = MenuRenderer(start_menu);
        MenuRenderer pause_menu_renderer = MenuRenderer(pause_menu);
        MenuRenderer end_menu_renderer = MenuRenderer(end_menu);
        GameRenderer game_renderer = GameRenderer(game);

        // TODO: add FRAME_INTERVAL macro instead of magic number

        start_menu.activate();
        start_menu_renderer.init();
        while (start_menu.is_activated())
        {
            key = getch();
            control(key, game, start_menu, pause_menu, end_menu);

            if (!game.is_activated())
            {
                start_menu_renderer.draw();
                start_menu_renderer.render();
                usleep(10000);
                continue;
            }

            game_renderer.init();
            while (game.is_activated())
            {
                key = getch();
                control(key, game, start_menu, pause_menu, end_menu);
                if (game.is_game_over())
                {
                    game.deactivate();
                    end_menu.activate();
                    break;
                }

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
                    control(key, game, start_menu, pause_menu, end_menu);
                    pause_menu_renderer.draw();
                    pause_menu_renderer.render();
                    usleep(10000);
                }
                game_renderer.init();
            }

            end_menu_renderer.init();
            while (end_menu.is_activated())
            {
                key = getch();
                control(key, game, start_menu, pause_menu, end_menu);
                end_menu_renderer.draw();
                end_menu_renderer.render();
                usleep(10000);
            }
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
