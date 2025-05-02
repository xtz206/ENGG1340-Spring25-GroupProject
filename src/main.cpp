#include <iostream>
#include <string>
#include <ncurses.h>
#include <unistd.h>

#include "game.h"
#include "menu.h"
#include "render.h"
#include "loader.h"
#include "saver.h"

/*The init function initializes terminal interface settings. 
It configures locale settings, enables screen management libraries (such as initscr and start_color), disables echo input, 
hides the cursor, and sets non-blocking input and keyboard support.
*/
/*initscr is a function from the ncurses library that initializes the screen and sets up the program's default window. 
It returns a pointer to the main window (of type WINDOW*), 
preparing the environment for subsequent ncurses screen operations.
*/
/*noecho is a function returning type int. In contexts like ncurses-based libraries, it disables terminal echo functionality, 
preventing input characters from being displayed during user input.
*/
/*curs_set is a function that controls terminal cursor visibility. 
It accepts an integer parameter and returns the previous cursor state, commonly used to manipulate terminal display behavior.
*/
//start_color is a function returning type int. It typically initializes terminal color capabilities.
/*The nodelay function sets the input mode for a specified window (WINDOW*). 
A boolean parameter controls whether input is non-blocking: true enables non-blocking mode, false enables blocking mode.
*/
/*The keypad function enables or disables function key support for a specified window (WINDOW*). 
The second boolean parameter determines the state: true enables function key support, false disables it.
*/

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

/*The main function serves as the entry point of the program. 
It initializes game resources (such as menus, renderers, game objects, etc.), 
and processes user input through loops to control menu navigation, game logic, and state transitions, 
thereby implementing the complete game flow.
*/
/*
Implements a core framework for a strategic defense game "Missile Commander," 
featuring menu navigation, game state management, and resource handling. 
(1)System Initialization
Uses init() for global setup (likely graphics/resources)
Implements exception handling via try block (though missing catch in shown snippet)
(2)Resource Management
Loader class handles asset loading (map sizes, cities, backgrounds)
SaveDumper/SaveLoader manage game state persistence
(3)Menu Architecture
Modular BasicMenu system for different contexts:
Start menu (game launch/load/tutorial/quit)
Difficulty selection (easy/normal/hard)
Pause menu (resume/save/quit)
Specialized menus (tech tree, tutorial, save slots)
Dedicated renderers (BasicMenuRenderer, TutorialMenuRenderer) for visual presentation
(4)Game Core
Game object constructed with loaded resources
Operational menus (OperationMenu, TechMenu) for in-game actions
Separation of logic (game state) and presentation (renderers)
(5)Key Features
Multiple interactive layers: Main game, tech research, tutorials
Input handling via short key variable (WIP in shown snippet)
Debug infrastructure with temporary "DEBUG" menu option
Localization-ready design (planned string replacement via vectors/maps)
*/
int main(void)
{
    try
    {
        init();

        short key;
        Loader loader = Loader();
        // TODO: store menu title and buttons in separate file instead of hardcoding
        // TODO: button name localization
        //       replace the strings with a vector<string> or map<string, string> or macro
        BasicMenu start_menu = BasicMenu("MISSILE COMMANDER", {"START THE GAME", "LOAD GAME", "TUTORIAL", "QUIT"});
        BasicMenu level_menu = BasicMenu("SELECT DIFFICULTY", {"EASY", "NORMAL", "HARD"});
        BasicMenu pause_menu = BasicMenu("PAUSED", {"RESUME", "RETURN TO MENU", "SAVE GAME", "QUIT"});
        BasicMenu save_menu = BasicMenu("SAVE GAME", {"SLOT 1", "SLOT 2", "SLOT 3", "RETURN TO MENU"});
        BasicMenu load_menu = BasicMenu("LOAD GAME", {"SLOT 1", "SLOT 2", "SLOT 3", "RETURN"});
        // DEBUG: the 'DEBUG' button is just for testing, remove later
        // This is the menu page for the game.
        //It introduces basic buttons for the game,enabling users to get familiar with basic steps like
        // starting the game, loading a game, and quitting the game.
        // also basic actions including switch,repair,launching missile and research.
        BasicMenu end_menu = BasicMenu("GAME OVER", {"DEBUG", "RETURN TO MENU", "QUIT"});
        Game game = Game(loader.load_size(), loader.load_cities(), loader.load_background());
        OperationMenu operation_menu = OperationMenu(game);
        TechMenu tech_menu = TechMenu(game.get_tech_tree(), "Return to Game");
        TutorialMenu tutorial_menu = TutorialMenu({{
                                                       "===== Controls =====",
                                                       "W/A/S/D  - Move Cursor",
                                                       "Tab      - Switch Cities",
                                                       "Enter    - Pass Turn",
                                                       "F        - Repair City",
                                                       "L        - Launch Missile",
                                                       "R        - Research Tech",
                                                       "ESC      - Pause/Open Menu",
                                                   },
                                                   {"===== Objectives =====",
                                                    "Protect cities from missiles!",
                                                    "Use research to unlock defenses."}});

        BasicMenuRenderer start_menu_renderer = BasicMenuRenderer(start_menu);
        BasicMenuRenderer level_menu_renderer = BasicMenuRenderer(level_menu);
        TutorialMenuRenderer tutorial_menu_renderer = TutorialMenuRenderer(tutorial_menu);
        BasicMenuRenderer pause_menu_renderer = BasicMenuRenderer(pause_menu);
        BasicMenuRenderer save_menu_renderer = BasicMenuRenderer(save_menu);
        BasicMenuRenderer load_menu_renderer = BasicMenuRenderer(load_menu);
        BasicMenuRenderer end_menu_renderer = BasicMenuRenderer(end_menu);
        GameRenderer game_renderer = GameRenderer(game, operation_menu);
        TechMenuRenderer tech_menu_renderer = TechMenuRenderer(tech_menu);

        SaveDumper save_dumper = SaveDumper(game);
        SaveLoader save_loader = SaveLoader(game);

        start_menu.activate();
        start_menu_renderer.init();

        /*A. Menu System
Start Menu: Initial screen with options to start game, load game, view tutorial, or quit
Level Menu: Difficulty selection (Easy/Normal/Hard)
Tutorial Menu: Multi-page instructions with navigation
Pause Menu: In-game options (Resume/Return to Menu/Save/Quit)
Tech Menu: Technology research interface
Save/Load Menus: Slot-based save system
End Menu: Post-game options

B. Game State
Handles core gameplay through Game class methods:
Cursor movement (WASD keys)
City operations (1-0 keys for quick city selection)
Missile/bomb management
Turn progression (Space key)
Research system
Iron Curtain activation
Game over checks

3. Input Handling
Uses getch() for keyboard input
Common Controls:
WASD/QE for navigation
Enter for selection
ESC (ASCII 27) for back/cancel
P for pause
Number keys 1-0 for quick city selection
Special keys for gameplay actions (Space, R, F, B, L)

4. State Transitions
Uses activate()/deactivate() methods to switch between states
Maintains strict hierarchy:
Start → Level → Game ↔ Pause ↔ Tech ↔ Save/Load
Start → Tutorial
Game → End
5. Rendering System
Each menu has its own renderer (*_renderer)

Implements double buffering pattern:
cpp
draw();    // Prepare frame
render();  // Display frame
Uses usleep(10000) for 10ms frame pacing

6. Key Technical Features
Game Serialization: save_dumper and save_loader handle save files
Difficulty System: set_difficulty() modifies enemy stats
Research System: Tech tree integration with gameplay effects
Super Weapons: Multiple bomb types with build/launch phases
Defensive Systems: City repair and Iron Curtain mechanics

7. Error Handling
Wrapped in try-catch block for exceptions
Proper terminal cleanup with endwin()

8. Notable Patterns
Chain of Responsibility: Input handling propagates through active state
Observer Pattern: Menu renderers observe menu states
Command Pattern: Key presses map to game actions
        */
        while (true)
        {
            if (start_menu.is_activated())
            {
                start_menu_renderer.init();
                while (start_menu.is_activated())
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
                            start_menu.deactivate();
                            level_menu.activate();
                        }
                        else if (start_menu.get_item() == "LOAD GAME")
                        {
                            start_menu.deactivate();
                            load_menu.activate();
                        }
                        else if (start_menu.get_item() == "TUTORIAL")
                        {
                            start_menu.deactivate();
                            tutorial_menu.activate();
                        }
                        else if (start_menu.get_item() == "QUIT")
                        {
                            start_menu.deactivate();
                        }
                        break;

                    case '\033':
                        start_menu.deactivate();
                        break;
                    }
                    start_menu_renderer.draw();
                    start_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (level_menu.is_activated())
            {
                level_menu_renderer.init();
                while (level_menu.is_activated())
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
                            game.set_difficulty(1);
                            level_menu.deactivate();
                            game.activate();
                        }
                        else if (level_menu.get_item() == "NORMAL")
                        {
                            game.set_difficulty(2);
                            level_menu.deactivate();
                            game.activate();
                        }
                        else if (level_menu.get_item() == "HARD")
                        {
                            game.set_difficulty(3);
                            level_menu.deactivate();
                            game.activate();
                        }
                        break;

                    case '\033':
                        level_menu.deactivate();
                        break;
                    }
                    level_menu_renderer.draw();
                    level_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (tutorial_menu.is_activated())
            {
                tutorial_menu_renderer.init();
                while (tutorial_menu.is_activated())
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
                            tutorial_menu.deactivate();
                            start_menu.activate();
                        }
                        break;

                    case 'p':
                        tutorial_menu.deactivate();
                        pause_menu.activate();
                        break;

                    case '\033':
                        tutorial_menu.deactivate();
                        break;
                    }
                    tutorial_menu_renderer.draw();
                    tutorial_menu_renderer.render();
                    usleep(10000);
                }
            }

            else if (game.is_activated())
            {
                game_renderer.init();
                while (game.is_activated())
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
                            game.deactivate();
                            tech_menu.activate();
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
                        game.deactivate();
                        pause_menu.activate();
                        break;

                    // NOTE: keyboard shortcuts for common operations
                    case 'r':
                        game.deactivate();
                        tech_menu.activate();
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
                        game.deactivate();
                        break;
                    }
                    if (game.is_game_over())
                    {
                        game.deactivate();
                        end_menu.activate();
                        break;
                    }
                    operation_menu.update_items();
                    game_renderer.draw();
                    game_renderer.render();
                    usleep(10000);
                }
            }
            else if (pause_menu.is_activated())
            {
                pause_menu_renderer.init();
                while (pause_menu.is_activated())
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
                            pause_menu.deactivate();
                            game.activate();
                        }
                        else if (pause_menu.get_item() == "RETURN TO MENU")
                        {
                            pause_menu.deactivate();
                            game.deactivate();
                            start_menu.activate();
                        }
                        else if (pause_menu.get_item() == "SAVE GAME")
                        {
                            pause_menu.deactivate();
                            save_menu.activate();
                        }
                        else if (pause_menu.get_item() == "QUIT")
                        {
                            pause_menu.deactivate();
                        }
                        break;

                    case 'p':
                        pause_menu.deactivate();
                        game.activate();
                        break;

                    case '\033':
                        pause_menu.deactivate();
                        break;
                    }
                    pause_menu_renderer.draw();
                    pause_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (tech_menu.is_activated())
            {
                tech_menu_renderer.init();
                while (tech_menu.is_activated())
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
                        if (tech_menu.get_item() == "Return to Game")
                        {
                            tech_menu.deactivate();
                            game.activate();
                        }
                        else if (tech_menu.check_tech_node())
                        {
                            game.start_research(tech_menu.get_tech_node());
                            game.check_research();
                        }
                        break;

                    case 'r':
                        tech_menu.deactivate();
                        game.activate();
                        break;

                    case '\033':
                        tech_menu.deactivate();
                        break;
                    }
                    tech_menu_renderer.draw();
                    tech_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (save_menu.is_activated())
            {
                save_menu_renderer.init();
                while (save_menu.is_activated())
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
                        // TODO: overwrite warning prompt
                        if (save_menu.get_item() == "SLOT 1")
                        {
                            if (!save_dumper.save_game("1"))
                            {
                                save_dumper.save_game("1", true);
                            }
                            save_menu.deactivate();
                            pause_menu.activate();
                        }
                        else if (save_menu.get_item() == "SLOT 2")
                        {
                            if (!save_dumper.save_game("2"))
                            {
                                save_dumper.save_game("2", true);
                            }
                            save_menu.deactivate();
                            pause_menu.activate();
                        }
                        else if (save_menu.get_item() == "SLOT 3")
                        {
                            if (!save_dumper.save_game("3"))
                            {
                                save_dumper.save_game("3", true);
                            }
                            save_menu.deactivate();
                            pause_menu.activate();
                        }
                        else if (save_menu.get_item() == "RETURN TO MENU")
                        {
                            save_menu.deactivate();
                            pause_menu.activate();
                        }
                        break;

                    case '\033':
                        save_menu.deactivate();
                        break;
                    }
                    save_menu_renderer.draw();
                    save_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (load_menu.is_activated())
            {
                load_menu_renderer.init();
                while (load_menu.is_activated())
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
                        // TODO: empty save slot warning prompt
                        if (load_menu.get_item() == "SLOT 1")
                        {
                            save_loader.load_game("1");
                            load_menu.deactivate();
                            game.activate();
                        }
                        else if (load_menu.get_item() == "SLOT 2")
                        {
                            save_loader.load_game("2");
                            load_menu.deactivate();
                            game.activate();
                        }
                        else if (load_menu.get_item() == "SLOT 3")
                        {
                            save_loader.load_game("3");
                            load_menu.deactivate();
                            game.activate();
                        }
                        else if (load_menu.get_item() == "RETURN TO MENU")
                        {
                            load_menu.deactivate();
                            start_menu.activate();
                        }
                        break;

                    case '\033':
                        load_menu.deactivate();
                        break;
                    }
                    load_menu_renderer.draw();
                    load_menu_renderer.render();
                    usleep(10000);
                }
            }
            else if (end_menu.is_activated())
            {
                end_menu_renderer.init();
                while (end_menu.is_activated())
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
                            end_menu.deactivate();
                            start_menu.activate();
                        }
                        else if (end_menu.get_item() == "QUIT")
                        {
                            end_menu.deactivate();
                        }
                        break;

                    case '\033':
                        end_menu.deactivate();
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
