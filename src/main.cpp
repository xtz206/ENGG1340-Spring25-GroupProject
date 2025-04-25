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

void control(short key, Game &game, Menu &start_menu, Menu &pause_menu, Menu &end_menu, TechMenu &tech_menu)
{
    if (start_menu.is_activated())
    {
        switch (key)
        {
        case 'w':
            start_menu.move_cursor(-1);
            return;

        case 's':
            start_menu.move_cursor(1);
            return;

        case '\n':
            if (start_menu.get_cursor() == 0)
            {
                start_menu.deactivate();
                game.activate();
            }
            else if (start_menu.get_cursor() == 1)
            {
                start_menu.deactivate();
            }
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

        case 'r':
            game.deactivate();
            tech_menu.activate();
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
        case 'w':
            pause_menu.move_cursor(-1);
            return;
        case 's':
            pause_menu.move_cursor(1);
            return;

        case '\n':
            if (pause_menu.get_cursor() == 0)
            {
                pause_menu.deactivate();
                game.activate();
            }
            else if (pause_menu.get_cursor() == 1)
            {
                pause_menu.deactivate();
                game.deactivate();
                start_menu.activate();
            }
            else if (pause_menu.get_cursor() == 2)
            {
                pause_menu.deactivate();
            }
            return;

        case '\033':
            pause_menu.deactivate();
            game.activate();
            return;

        case 'q':
            pause_menu.deactivate();
            return;
        }
    }
    else if (tech_menu.is_activated())
    {
        switch (key)
        {
        case 'w':
            tech_menu.move_cursor(-1);
            return;
        case 's':
            tech_menu.move_cursor(1);
            return;

        case '\n':
            // TODO: START RESEARCH HERE
            return;

        case 'r':
        case '\033':
            tech_menu.deactivate();
            game.activate();
            return;

        case 'q':
            tech_menu.deactivate();
            return;
        }
    }

    else if (end_menu.is_activated())
    {
        switch (key)
        {
        case 'w':
            end_menu.move_cursor(-1);
            return;
        case 's':
            end_menu.move_cursor(1);
            return;

        case '\n':
            if (end_menu.get_cursor() == 1)
            {
                end_menu.deactivate();
                start_menu.activate();
            }
            else if (end_menu.get_cursor() == 2)
            {
                end_menu.deactivate();
            }
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
        // TODO: store menu title and buttons in separate file instead of hardcoding
        Menu start_menu = Menu("MISSILE COMMANDER", {"START THE GAME", "QUIT"});
        Menu pause_menu = Menu("PAUSED", {"RESUME", "RETURN TO MENU", "QUIT"});
        Menu end_menu = Menu("GAME OVER", {"DEBUG", "RETURN TO MENU", "QUIT"}); // DEBUG: the 'DEBUG' button is just for testing, remove later
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());
        TechMenu tech_menu = TechMenu(game.get_tech_tree());

        MenuRenderer start_menu_renderer = MenuRenderer(start_menu);
        MenuRenderer pause_menu_renderer = MenuRenderer(pause_menu);
        MenuRenderer end_menu_renderer = MenuRenderer(end_menu);
        GameRenderer game_renderer = GameRenderer(game);
        TechMenuRenderer tech_menu_renderer = TechMenuRenderer(tech_menu);

        // TODO: add FRAME_INTERVAL macro instead of magic number

        start_menu.activate();
        start_menu_renderer.init();
        while (start_menu.is_activated())
        {
            key = getch();
            control(key, game, start_menu, pause_menu, end_menu, tech_menu);

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
                control(key, game, start_menu, pause_menu, end_menu, tech_menu);
                if (game.is_game_over())
                {
                    game.deactivate();
                    end_menu.activate();
                    break;
                }

                if (!pause_menu.is_activated() && !tech_menu.is_activated())
                {
                    game_renderer.draw();
                    game_renderer.render();
                    usleep(10000);
                    continue;
                }
                else if (tech_menu.is_activated())
                {
                    tech_menu_renderer.init();
                    while (tech_menu.is_activated())
                    {
                        key = getch();
                        control(key, game, start_menu, pause_menu, end_menu, tech_menu);
                        tech_menu_renderer.draw();
                        tech_menu_renderer.render();
                        usleep(10000);
                    }
                }
                else
                {
                    pause_menu_renderer.init();
                    while (pause_menu.is_activated())
                    {
                        key = getch();
                        control(key, game, start_menu, pause_menu, end_menu, tech_menu);
                        pause_menu_renderer.draw();
                        pause_menu_renderer.render();
                        usleep(10000);
                    }
                }

                if (game.is_activated())
                    game_renderer.init();
            }

            if (!end_menu.is_activated())
            {
                start_menu_renderer.init();
                continue;
            }

            end_menu_renderer.init();
            while (end_menu.is_activated())
            {
                key = getch();
                control(key, game, start_menu, pause_menu, end_menu, tech_menu);
                end_menu_renderer.draw();
                end_menu_renderer.render();
                usleep(10000);
            }

            if (!start_menu.is_activated())
            {
                start_menu_renderer.init();
                continue;
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
