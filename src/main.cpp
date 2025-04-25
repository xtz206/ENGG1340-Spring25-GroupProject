#include <iostream>
#include <string>
#include <map>
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

void control(short key, Game &game, std::map<std::string, Menu> &menus)
{
    if (menus.at("start").is_activated())
    {
        switch (key)
        {
        case 'w':
            menus.at("start").move_cursor(-1);
            return;

        case 's':
            menus.at("start").move_cursor(1);
            return;

        case '\n':
            if (menus.at("start").get_cursor() == 0)
            {
                menus.at("start").deactivate();
                game.activate();
            }
            else if (menus.at("start").get_cursor() == 1)
            {
                menus.at("start").deactivate();
            }
            return;

        case 'q':
            menus.at("start").deactivate();
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
            menus.at("pause").activate();
            return;

        case 'r':
            game.start_research();
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
    else if (menus.at("pause").is_activated())
    {
        switch (key)
        {
        case 'w':
            menus.at("pause").move_cursor(-1);
            return;
        case 's':
            menus.at("pause").move_cursor(1);
            return;

        case '\n':
            if (menus.at("pause").get_cursor() == 0)
            {
                menus.at("pause").deactivate();
                game.activate();
            }
            else if (menus.at("pause").get_cursor() == 1)
            {
                menus.at("pause").deactivate();
                game.deactivate();
                menus.at("start").activate();
            }
            else if (menus.at("pause").get_cursor() == 2)
            {
                menus.at("pause").deactivate();
            }
            return;

        case '\033':
            menus.at("pause").deactivate();
            game.activate();
            return;

        case 'q':
            menus.at("pause").deactivate();
            return;
        }
    }
    else if (menus.at("end").is_activated())
    {
        switch (key)
        {
        case 'w':
            menus.at("end").move_cursor(-1);
            return;
        case 's':
            menus.at("end").move_cursor(1);
            return;

        case '\n':
            if (menus.at("end").get_cursor() == 1)
            {
                menus.at("end").deactivate();
                menus.at("start").activate();
            }
            else if (menus.at("end").get_cursor() == 2)
            {
                menus.at("end").deactivate();
            }
            return;

        case 'q':
            menus.at("end").deactivate();
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
        // TODO: store menu title and buttons in separate file instead of hardcoding
        std::map<std::string, Menu> menus = {
            {"start", Menu("MISSILE COMMANDER", {"START THE GAME", "QUIT"})},
            {"pause", Menu("PAUSED", {"RESUME", "RETURN TO MENU", "QUIT"})},
            {"end", Menu("GAME OVER", {"DEBUG", "RETURN TO MENU", "QUIT"})}
            // DEBUG: the 'DEBUG' button is just for testing, remove later
        };
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());

        MenuRenderer start_menu_renderer = MenuRenderer(menus.at("start"));
        MenuRenderer pause_menu_renderer = MenuRenderer(menus.at("pause"));
        MenuRenderer end_menu_renderer = MenuRenderer(menus.at("end"));
        GameRenderer game_renderer = GameRenderer(game);

        // TODO: add FRAME_INTERVAL macro instead of magic number

        menus.at("start").activate();
        start_menu_renderer.init();
        while (menus.at("start").is_activated())
        {
            key = getch();
            control(key, game, menus);

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
                control(key, game, menus);
                if (game.is_game_over())
                {
                    game.deactivate();
                    menus.at("end").activate();
                    break;
                }

                if (!menus.at("pause").is_activated())
                {
                    game_renderer.draw();
                    game_renderer.render();
                    usleep(10000);
                    continue;
                }

                pause_menu_renderer.init();
                while (menus.at("pause").is_activated())
                {
                    key = getch();
                    control(key, game, menus);
                    pause_menu_renderer.draw();
                    pause_menu_renderer.render();
                    usleep(10000);
                }
                if (game.is_activated())
                    game_renderer.init();
            }

            if (!menus.at("end").is_activated())
            {
                start_menu_renderer.init();
                continue;
            }

            end_menu_renderer.init();
            while (menus.at("end").is_activated())
            {
                key = getch();
                control(key, game, menus);
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
