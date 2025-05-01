#include <iostream>
#include <string>
#include <ncurses.h>
#include <unistd.h>

#include "game.h"
#include "menu.h"
#include "render.h"
#include "saver.h"
#include "utils.h"

enum class Stage
{
    TITLE_MENU,
    START_MENU,
    LEVEL_MENU,
    TUTORIAL_MENU,
    GAME,
    TECH_MENU,
    PAUSE_MENU,
    SAVE_MENU,
    LOAD_MENU,
    END_MENU,
    QUIT
};

void init(void)
{
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    curs_set(0);
    start_color();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_YELLOW);
    
}

int main(void)
{
    try
    {
        init();

        short key;
        Stage stage = Stage::TITLE_MENU;

        Game game = Game();
        SaveDumper save_dumper = SaveDumper(game);
        SaveLoader save_loader = SaveLoader(game);
        AssetLoader asset_loader = AssetLoader(game);
        asset_loader.load_general();

        TitleMenu title_menu = TitleMenu(asset_loader.load_title(), "PRESS ANY KEY TO START");
        BasicMenu start_menu = BasicMenu("START MENU", {"START THE GAME", "LOAD GAME", "TUTORIAL", "QUIT"});
        BasicMenu level_menu = BasicMenu("SELECT DIFFICULTY", {"EASY", "NORMAL", "HARD"});
        BasicMenu pause_menu = BasicMenu("PAUSED", {"RESUME", "RETURN TO MENU", "SAVE GAME", "QUIT"});
        SaveMenu save_menu = SaveMenu("SAVE GAME", save_dumper);
        LoadMenu load_menu = LoadMenu("LOAD GAME", save_loader);
        BasicMenu end_menu = BasicMenu("GAME END", {"RETURN TO MENU", "QUIT"});
        OperationMenu operation_menu = OperationMenu(game);
        TechMenu tech_menu = TechMenu(game.get_tech_tree(), "RETURN TO GAME");
        TutorialMenu tutorial_menu = TutorialMenu();

        TitleMenuRenderer title_menu_renderer = TitleMenuRenderer(title_menu, Size(10, 100));
        BasicMenuRenderer start_menu_renderer = BasicMenuRenderer(start_menu, Size(10, 30));
        BasicMenuRenderer level_menu_renderer = BasicMenuRenderer(level_menu, Size(10, 30));
        BasicMenuRenderer pause_menu_renderer = BasicMenuRenderer(pause_menu, Size(10, 30));
        TutorialMenuRenderer tutorial_menu_renderer = TutorialMenuRenderer(tutorial_menu, Size(15, 40), Size(5, 40));
        SaveMenuRenderer save_menu_renderer = SaveMenuRenderer(save_menu, Size(10, 30));
        SaveMenuRenderer load_menu_renderer = SaveMenuRenderer(load_menu, Size(10, 30));
        EndMenuRenderer end_menu_renderer = EndMenuRenderer(game, end_menu, Size(10, 30), Size(5, 30));
        GameRenderer game_renderer = GameRenderer(game, operation_menu, Size(10, 30), {6, 6, 4, 4});
        TechMenuRenderer tech_menu_renderer = TechMenuRenderer(tech_menu, Size(10, 40), Size(10, 40));

        while (true)
        {
            if (stage == Stage::TITLE_MENU)
            {
                title_menu_renderer.init();
                while (stage == Stage::TITLE_MENU)
                {
                    key = getch();

                    switch (key)
                    {
                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    case ERR:
                        break;
                    default:
                        stage = Stage::START_MENU;
                        break;
                    }
                    title_menu_renderer.draw();
                    title_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::START_MENU)
            {
                start_menu_renderer.init();
                while (stage == Stage::START_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        start_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        start_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (start_menu.get_item() == "START THE GAME")
                        {
                            stage = Stage::LEVEL_MENU;
                        }
                        else if (start_menu.get_item() == "LOAD GAME")
                        {
                            stage = Stage::LOAD_MENU;
                        }
                        else if (start_menu.get_item() == "TUTORIAL")
                        {
                            stage = Stage::TUTORIAL_MENU;
                        }
                        else if (start_menu.get_item() == "QUIT")
                        {
                            stage = Stage::QUIT;
                        }
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    start_menu_renderer.draw();
                    start_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::LEVEL_MENU)
            {
                level_menu_renderer.init();
                while (stage == Stage::LEVEL_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        level_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        level_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (level_menu.get_item() == "EASY")
                        {
                            asset_loader.reset();
                            game.set_difficulty(1);
                            stage = Stage::GAME;
                        }
                        else if (level_menu.get_item() == "NORMAL")
                        {
                            asset_loader.reset();
                            game.set_difficulty(2);
                            stage = Stage::GAME;
                        }
                        else if (level_menu.get_item() == "HARD")
                        {
                            asset_loader.reset();
                            game.set_difficulty(3);
                            stage = Stage::GAME;
                        }
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    level_menu_renderer.draw();
                    level_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::TUTORIAL_MENU)
            {
                tutorial_menu_renderer.init();
                while (stage == Stage::TUTORIAL_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                        tutorial_menu.move_cursor(-1);
                        break;
                    case 'a':
                    case 'q':
                        tutorial_menu.prev_page();
                        break;
                    case 's':
                        tutorial_menu.move_cursor(1);
                        break;
                    case 'd':
                    case 'e':
                        tutorial_menu.next_page();
                        break;

                    case '\n':
                        if (tutorial_menu.get_item() == "RETURN TO MENU")
                        {
                            stage = Stage::START_MENU;
                        }
                        else if (tutorial_menu.get_item() == "NEXT PAGE")
                        {
                            tutorial_menu.next_page();
                        }
                        else if (tutorial_menu.get_item() == "PREV PAGE")
                        {
                            tutorial_menu.prev_page();
                        }
                        break;

                    case 'p':
                        stage = Stage::START_MENU;
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    tutorial_menu_renderer.draw();
                    tutorial_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::GAME)
            {
                game_renderer.init();
                while (stage == Stage::GAME)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                        game.move_cursor(Position(-1, 0));
                        break;
                    case 's':
                        game.move_cursor(Position(1, 0));
                        break;
                    case 'a':
                        game.move_cursor(Position(0, -1));
                        break;
                    case 'd':
                        game.move_cursor(Position(0, 1));
                        break;

                    case 'q':
                        operation_menu.move_cursor(-1);
                        break;
                    case 'e':
                        operation_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (operation_menu.get_item() == "RESEARCH")
                        {
                            stage = Stage::TECH_MENU;
                        }
                        else if (operation_menu.get_item() == "FIX")
                        {
                            game.fix_city();
                        }
                        else if (operation_menu.get_item() == "BUILD CRUISE")
                        {
                            game.build_cruise();
                        }
                        else if (operation_menu.get_item() == "LAUNCH CRUISE")
                        {
                            game.launch_cruise();
                        }
                        else if (operation_menu.get_item() == "BUILD STANDARD BOMB")
                        {
                            game.build_standard_bomb();
                        }
                        else if (operation_menu.get_item() == "LAUNCH STANDARD BOMB")
                        {
                            game.launch_standard_bomb();
                        }
                        else if (operation_menu.get_item() == "BUILD DIRTY BOMB")
                        {
                            game.build_dirty_bomb();
                        }
                        else if (operation_menu.get_item() == "LAUNCH DIRTY BOMB")
                        {
                            game.launch_dirty_bomb();
                        }
                        else if (operation_menu.get_item() == "BUILD HYDROGEN BOMB")
                        {
                            game.build_hydrogen_bomb();
                        }
                        else if (operation_menu.get_item() == "LAUNCH HYDROGEN BOMB")
                        {
                            game.launch_hydrogen_bomb();
                        }
                        else if (operation_menu.get_item() == "ACTIVATE IRON CURTAIN")
                        {
                            game.activate_iron_curtain();
                        }
                        break;

                    case '1':
                        game.move_cursor_to_city(0);
                        break;
                    case '2':
                        game.move_cursor_to_city(1);
                        break;
                    case '3':
                        game.move_cursor_to_city(2);
                        break;
                    case '4':
                        game.move_cursor_to_city(3);
                        break;
                    case '5':
                        game.move_cursor_to_city(4);
                        break;
                    case '6':
                        game.move_cursor_to_city(5);
                        break;
                    case '7':
                        game.move_cursor_to_city(6);
                        break;
                    case '8':
                        game.move_cursor_to_city(7);
                        break;
                    case '9':
                        game.move_cursor_to_city(8);
                        break;
                    case '0':
                        game.move_cursor_to_city(9);
                        break;

                    case ' ':
                        game.pass_turn();
                        break;
                    case 'p':
                        stage = Stage::PAUSE_MENU;
                        break;

                    // NOTE: keyboard shortcuts for common operations
                    case 'r':
                        stage = Stage::TECH_MENU;
                        break;
                    case 'f':
                        game.fix_city();
                        break;
                    case 'b':
                        game.build_cruise();
                        break;
                    case 'l':
                        game.launch_cruise();
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    if (game.check_game_over())
                    {
                        stage = Stage::END_MENU;
                        break;
                    }
                    operation_menu.update_items();
                    game_renderer.draw();
                    game_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::PAUSE_MENU)
            {
                pause_menu_renderer.init();
                while (stage == Stage::PAUSE_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        pause_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        pause_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (pause_menu.get_item() == "RESUME")
                        {
                            stage = Stage::GAME;
                        }
                        else if (pause_menu.get_item() == "RETURN TO MENU")
                        {
                            stage = Stage::START_MENU;
                        }
                        else if (pause_menu.get_item() == "SAVE GAME")
                        {
                            stage = Stage::SAVE_MENU;
                        }
                        else if (pause_menu.get_item() == "QUIT")
                        {
                            stage = Stage::QUIT;
                        }
                        break;

                    case 'p':
                        stage = Stage::GAME;
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    pause_menu_renderer.draw();
                    pause_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::TECH_MENU)
            {
                tech_menu_renderer.init();
                while (stage == Stage::TECH_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        tech_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        tech_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (tech_menu.get_item() == "RETURN TO GAME")
                        {
                            stage = Stage::GAME;
                        }
                        else if (tech_menu.check_tech_node())
                        {
                            game.start_research(tech_menu.get_tech_node());
                            game.check_research();
                        }
                        break;

                    case 'r':
                        stage = Stage::GAME;
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    tech_menu_renderer.draw();
                    tech_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::SAVE_MENU)
            {
                save_menu_renderer.init();
                while (stage == Stage::SAVE_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        save_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        save_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (save_menu.get_item() == "SLOT 1 EMPTY" || save_menu.get_item() == "SLOT 1  FULL")
                        {
                            save_dumper.save_game("1");
                            stage = Stage::PAUSE_MENU;
                        }
                        else if (save_menu.get_item() == "SLOT 2 EMPTY" || save_menu.get_item() == "SLOT 2  FULL")
                        {
                            save_dumper.save_game("2");
                            stage = Stage::PAUSE_MENU;
                        }
                        else if (save_menu.get_item() == "SLOT 3 EMPTY" || save_menu.get_item() == "SLOT 3  FULL")
                        {
                            save_dumper.save_game("3");
                            stage = Stage::PAUSE_MENU;
                        }
                        else if (save_menu.get_item() == "RETURN TO MENU")
                        {
                            stage = Stage::PAUSE_MENU;
                        }
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    save_menu.update_items();
                    save_menu_renderer.draw();
                    save_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::LOAD_MENU)
            {
                load_menu_renderer.init();
                while (stage == Stage::LOAD_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        load_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        load_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (load_menu.get_item() == "SLOT 1  FULL")
                        {
                            asset_loader.reset();
                            save_loader.load_game("1");
                            stage = Stage::GAME;
                        }
                        else if (load_menu.get_item() == "SLOT 2 FULL")
                        {
                            asset_loader.reset();
                            save_loader.load_game("2");
                            stage = Stage::GAME;
                        }
                        else if (load_menu.get_item() == "SLOT 3 FULL")
                        {
                            asset_loader.reset();
                            save_loader.load_game("3");
                            stage = Stage::GAME;
                        }
                        else if (load_menu.get_item() == "RETURN TO MENU")
                        {
                            stage = Stage::START_MENU;
                        }
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    load_menu.update_items();
                    load_menu_renderer.draw();
                    load_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (stage == Stage::END_MENU)
            {
                end_menu_renderer.init();
                while (stage == Stage::END_MENU)
                {
                    key = getch();
                    switch (key)
                    {
                    case 'w':
                    case 'a':
                    case 'q':
                        end_menu.move_cursor(-1);
                        break;
                    case 's':
                    case 'd':
                    case 'e':
                        end_menu.move_cursor(1);
                        break;

                    case '\n':
                        if (end_menu.get_item() == "RETURN TO MENU")
                        {
                            stage = Stage::START_MENU;
                        }
                        else if (end_menu.get_item() == "QUIT")
                        {
                            stage = Stage::QUIT;
                        }
                        break;

                    case '\033':
                        stage = Stage::QUIT;
                        break;
                    }
                    end_menu_renderer.draw();
                    end_menu_renderer.render();
                    usleep(10000);
                }
            }
            else
            {
                break;
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
