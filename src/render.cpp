/* render.cpp */
/* Implementation of rendering system for terminal-based game interface using ncurses */

// ------------------------ Header Includes ------------------------
#include <string>
#include <vector>
#include <fstream>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "render.h"

// ------------------------ Window Dimension Macros ------------------------
// TODO: make screen size variable to map size
#define TOTAL_LINES 30 // Total main window height
#define TOTAL_COLS 150 // Total main window width
#define MAP_LINES 18   // Game map area height
#define MAP_COLS 98      // Game map area width
#define INFO_LINES 7     // Height of information panels
#define INFO_COLS 49     // Width of information panels
#define OPERATION_LINES 9  // Operation menu height
#define OPERATION_COLS 40  // Feedback display height
#define FEEDBACK_LINES 9   /*similar to*/
#define FEEDBACK_COLS 56   /*the operation part*/
#define MENU_LINES 10      //similar parameters 
#define MENU_COLS 40
#define TECH_LINES 10
#define TECH_COLS 80

// ------------------------ Renderer Base Class ------------------------
/// @brief Output debug information to specified line
/// @param str Debug message content
/// @paramline Line number on screen (0-based)
void Renderer::debug(const std::string &str, int line)
{
    mvwprintw(stdscr, line, 1, "%s", str.c_str());
}

// ------------------------ Basic Menu Renderer ------------------------
///@brief Initialize basic menu window structure
///@details Creates centered window with border and title
void BasicMenuRenderer::init(void)
{
    erase();// Clear previous screen content

    // Create bordered container window
    box_window = subwin(stdscr, MENU_LINES + 2, MENU_COLS + 2, (LINES - MENU_LINES - 2) / 2, (COLS - MENU_COLS - 2) / 2);// Parent window
    ///@param stdscr Height (including border)
    ///@param MENU_LINES + 2 MENU_COLS + 2 Width (including border)
    /// @param (LINES - MENU_LINES - 2) / 2 Vertical centering
    ///@param (COLS - MENU_COLS - 2) / 2 Horizontal centering
    // Create content window inside borders
    item_window = subwin(box_window, MENU_LINES, MENU_COLS, (LINES - MENU_LINES) / 2, (COLS - MENU_COLS) / 2);//similar to the box-window
    /*height,width,
    vertical position,
    horizontal position
    */
    box(box_window, 0, 0); // Draw border around container
    // Center-align menu title in border's top bar
    mvwprintw(box_window, 0, (MENU_COLS - menu.get_title().length() + 2) / 2, "%s", menu.get_title().c_str());
    ///@param 0 top line of border
    /*@param (MENU_COLS - menu.get_title().length() + 2) / 2,Centered position */
}

/// @brief Refresh menu display
void BasicMenuRenderer::render(void)
{
    wrefresh(item_window); // Update previous content
}

/// @brief Render menu items with selection highlighting
/// @details Implements scrollable menu with reverse video selection
void BasicMenuRenderer::draw(void)
{
    werase(item_window);// Clear previous content
    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            wattron(item_window, A_REVERSE); // Enable reverse video
            mvwprintw(item_window, index, (MENU_COLS - menu.get_item(index).length()) / 2, "%s", menu.get_item(index).c_str());
            wattroff(item_window, A_REVERSE); // Disable reverse video
        }
        else
        {
            mvwprintw(item_window, index, (MENU_COLS - menu.get_item(index).length()) / 2, "%s", menu.get_item(index).c_str());
        }
    }
}

void TutorialMenuRenderer::init(void)
{
    erase();
    
    box_window = subwin(stdscr, MENU_LINES + 2, MENU_COLS + 2, (LINES - MENU_LINES - 2) / 2, (COLS - MENU_COLS - 2) / 2);
    item_window = subwin(box_window, MENU_LINES, MENU_COLS, (LINES - MENU_LINES) / 2, (COLS - MENU_COLS) / 2);
    box(box_window, 0, 0);
    mvwprintw(box_window, 0, (MENU_COLS - menu.get_title().length() + 2) / 2, "%s", menu.get_title().c_str());
}

void TutorialMenuRenderer::render(void)
{
    wrefresh(item_window);
}

void TutorialMenuRenderer::draw(void)
{
    werase(item_window);
    const std::vector<std::string> &page = menu.get_content();
    for (size_t index = 0; index < page.size(); index++)
    {
        mvwprintw(item_window, index, 0, "%s", page.at(index).c_str());
    }
    std::string page_info = menu.get_page_info();
    mvwprintw(item_window, MENU_LINES - 1, (MENU_COLS - page_info.length()) / 2, "%s", page_info.c_str());
}

// ------------------------ Technology Menu Renderer ------------------------
/// @brief Initialize dual-pane tech tree interface
/// @details Creates split window with list on top and description below
void TechMenuRenderer::init(void)
{
    erase();// Clear previous content
    
    // Main container window
    box_window = subwin(stdscr, TECH_LINES * 2 + 3, TECH_COLS + 2, (LINES - TECH_LINES * 2 - 2) / 2, (COLS - TECH_COLS - 2) / 2);
    // Total height (2 panels + borders)
    // Total width
    // Vertical center
    // Horizontal center
    // Technology list pane (upper half)
    item_window = subwin(box_window, TECH_LINES, TECH_COLS, (LINES - TECH_LINES * 2 - 3) / 2 + 1, (COLS - TECH_COLS) / 2);
    // Below top border
    // Description pane (lower half)
    desc_window = subwin(box_window, TECH_LINES, TECH_COLS, (LINES - TECH_LINES * 2 - 3) / 2 + TECH_LINES + 2, (COLS - TECH_COLS) / 2);
    box(box_window, 0, 0); // Draw main border
    mvwprintw(box_window, 0, (TECH_COLS + 2 - menu.get_title().length()) / 2, "%s", menu.get_title().c_str());
    // Add horizontal separator between panes
    mvwhline(box_window, TECH_LINES + 1, 1, ACS_HLINE, TECH_COLS);
    mvwaddch(box_window, TECH_LINES + 1, 0, ACS_LTEE);             // Add horizontal separator between panes
    mvwaddch(box_window, TECH_LINES + 1, TECH_COLS + 1, ACS_RTEE); // Right T-junction
}

/// @brief Refresh tech menu display
void TechMenuRenderer::render(void)
{
    wrefresh(item_window);
    wrefresh(desc_window);
}

/// @brief Render tech tree items with selection highlighting
void TechMenuRenderer::draw()
{
    werase(item_window);
    werase(desc_window);

    for (int index = menu.get_offset(); index < menu.get_offset() + menu.get_limit(); index++)
    {
        if (index >= menu.get_items().size())
        {
            break;
        }
        if (index == menu.get_cursor())
        {
            wattron(item_window, A_REVERSE);
            mvwprintw(item_window, index - menu.get_offset(), 0, "%s", menu.get_item(index).c_str());
            wattroff(item_window, A_REVERSE);
        }
        else
        {
            mvwprintw(item_window, index - menu.get_offset(), 0, "%s", menu.get_item(index).c_str());
        }
    }

    std::vector<std::string> description = menu.get_item_description();
    for (size_t index = 0; index < description.size(); index++)
    {
        mvwprintw(desc_window, index, 0, "%s", description.at(index).c_str());
    }
}

// ------------------------ Game Main Renderer ------------------------
/// @brief Initialize complex game interface layout
/// @details Creates multiple subwindows for different game aspects:
/// - Map display
/// - Information panels
/// - Operation menu
/// - Feedback console
void GameRenderer::init(void)
{
    erase();// Clear screen
    
    /* Window hierarchy:
    +--------------------------------+----------------+
    |           map_window            | general_info   |
    |           (MAP_LINES x MAP_COLS)| (INFO_LINES x  |
    |                                 |  INFO_COLS)    |
    +---------------------------------+----------------+
    | operation_window                | feedback_window|
    | (OPERATION_LINES x OPERATION_COLS)               |
    +--------------------------------+-----------------+
    */

    // Main container window
    box_window = subwin(stdscr, TOTAL_LINES, TOTAL_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2);
    // Game map area
    map_window = subwin(box_window, MAP_LINES, MAP_COLS, (LINES - TOTAL_LINES) / 2 + 1, (COLS - TOTAL_COLS) / 2 + 1);
    // Information panels (right side)
    general_info_window = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + 1, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    selected_info_window = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + INFO_LINES + 2, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    // Technology and research panel
    tech_info_window = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + INFO_LINES * 2 + 3, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    super_weapon_info = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + INFO_LINES * 3 + 4, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    // Operation menu (left side)
    operation_window = subwin(box_window, OPERATION_LINES, OPERATION_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES + 2, (COLS - TOTAL_COLS) / 2 + 1);
    feedback_window = subwin(box_window, FEEDBACK_LINES, FEEDBACK_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES + 2, (COLS - TOTAL_COLS) / 2 + OPERATION_COLS + 3);

    // Draw interface decorations
    box(box_window, 0, 0); // Main border
    mvwhline(box_window, MAP_LINES + 1, 1, ACS_HLINE, MAP_COLS); // Horizontal separator
    mvwvline(box_window, 1, MAP_COLS + 1, ACS_VLINE, TOTAL_LINES - 2); // Vertical separator

    mvwaddch(box_window, 0, MAP_COLS + 1, ACS_TTEE);
    mvwaddch(box_window, TOTAL_LINES - 1, MAP_COLS + 1, ACS_BTEE);

    mvwaddch(box_window, MAP_LINES + 1, 0, ACS_LTEE);
    mvwaddch(box_window, MAP_LINES + 1, MAP_COLS + 1, ACS_RTEE);

    mvwvline(box_window, MAP_LINES + 2, OPERATION_COLS + 2, ACS_VLINE, OPERATION_LINES);
    mvwaddch(box_window, MAP_LINES + 1, OPERATION_COLS + 2, ACS_TTEE);
    mvwaddch(box_window, TOTAL_LINES - 1, OPERATION_COLS + 2, ACS_BTEE);

    mvwhline(box_window, INFO_LINES + 1, MAP_COLS + 2, ACS_HLINE, INFO_COLS);
    mvwaddch(box_window, INFO_LINES + 1, MAP_COLS + 1, ACS_LTEE);
    mvwaddch(box_window, INFO_LINES + 1, TOTAL_COLS - 1, ACS_RTEE);

    mvwhline(box_window, INFO_LINES * 2 + 2, MAP_COLS + 2, ACS_HLINE, INFO_COLS);
    mvwaddch(box_window, INFO_LINES * 2 + 2, MAP_COLS + 1, ACS_LTEE);
    mvwaddch(box_window, INFO_LINES * 2 + 2, TOTAL_COLS - 1, ACS_RTEE);

    mvwhline(box_window, INFO_LINES * 3 + 3, MAP_COLS + 2, ACS_HLINE, INFO_COLS);
    mvwaddch(box_window, INFO_LINES * 3 + 3, MAP_COLS + 1, ACS_LTEE);
    mvwaddch(box_window, INFO_LINES * 3 + 3, TOTAL_COLS - 1, ACS_RTEE);

    // mvwprintw(box_window, 0, (TOTAL_COLS - 2 - 2) / 2, "Missile Command");
    // Add panel titles
    mvwprintw(box_window, 0, 2, "Map");
    mvwprintw(box_window, 0, 2 + MAP_COLS + 2, "General");
    mvwprintw(box_window, INFO_LINES + 1, 2 + MAP_COLS + 2, "City & Missile");
    mvwprintw(box_window, INFO_LINES * 2 + 2, 2 + MAP_COLS + 2, "Technology & Research");
    mvwprintw(box_window, INFO_LINES * 3 + 3, 2 + MAP_COLS + 2, "Super Weapon");
    mvwprintw(box_window, MAP_LINES + 1, 2, "Operation");
    mvwprintw(box_window, MAP_LINES + 1, 2 + OPERATION_COLS + 2, "Feedback");
}

/// @brief Refresh game interface display
void GameRenderer::render(void)
{
    wrefresh(map_window);
    wrefresh(general_info_window);
    wrefresh(selected_info_window);
    wrefresh(tech_info_window);
    wrefresh(super_weapon_info);
    wrefresh(operation_window);
    wrefresh(feedback_window);
}

/// @brief Update all game interface elements
void GameRenderer::draw(void)
{   
    // Clear all subwindows
    werase(map_window);
    werase(general_info_window);
    // ... (clear other windows)
    werase(selected_info_window);
    werase(tech_info_window);
    werase(super_weapon_info);
    werase(operation_window);
    werase(feedback_window);

    // NOTE: draw map window
    /* Draw game map elements */
    // Background terrain
    for (int index = 0; index < game.get_background().size(); index++)
    {
        mvwprintw(map_window, index, 0, "%s", game.get_background().at(index).c_str());
    }

    // Active missiles
    for (auto missile : game.get_missiles())
    {
        if (!game.is_in_map(missile->get_position()))
        {
            continue;
        }
        if (missile->get_is_exploded())
        {
            continue;
        }
        std::string direction;
        switch (missile->get_direction())
        {
        case MissileDirection::A://hit the object
            direction = "O";
            break;
        case MissileDirection::N: // North
            direction = "↑";
            break;
        case MissileDirection::NE:// Northeast
            direction = "↗";
            break;
        case MissileDirection::E://East
            direction = "→";
            break;
        case MissileDirection::SE://Southeast
            direction = "↘";
            break;
        case MissileDirection::S://South
            direction = "↓";
            break;
        case MissileDirection::SW://Southwest
            direction = "↙";
            break;
        case MissileDirection::W://West
            direction = "←";
            break;
        case MissileDirection::NW://Northwesr
            direction = "↖";
            break;
        case MissileDirection::U://Unknown
            direction = "x";
            break;
        default:
            break;
        }
        mvwprintw(map_window, missile->get_position().y, missile->get_position().x, "%s", direction.c_str());
    }
    mvwprintw(map_window, game.get_cursor().y, game.get_cursor().x, "X");

    // NOTE: draw info windows
    // Active missiles
    std::vector<std::string> info;
    info = game.get_general_info();
    for (size_t index = 0; index < info.size(); index++)
    {
        mvwprintw(general_info_window, index, 0, "%s", info.at(index).c_str());
    }
    int next_line = info.size();
    mvwprintw(general_info_window, next_line, 0, "Score: %d", game.get_score());
    info = game.get_selected_info();
    for (size_t index = 0; index < info.size(); index++)
    {
        mvwprintw(selected_info_window, index, 0, "%s", info.at(index).c_str());
    }
    info = game.get_tech_info();
    for (size_t index = 0; index < info.size(); index++)
    {
        mvwprintw(tech_info_window, index, 0, "%s", info.at(index).c_str());
    }
    info = game.get_super_weapon_info();
    for (size_t index = 0; index < info.size(); index++)
    {
        mvwprintw(super_weapon_info, index, 0, "%s", info.at(index).c_str());
    }

    // NOTE: draw operation window
    for (int index = menu.get_offset(); index < menu.get_offset() + menu.get_limit(); index++)
    {
        if (index >= menu.get_items().size())
        {
            break;
        }
        if (index == menu.get_cursor())
        {
            wattron(operation_window, A_REVERSE);
            mvwprintw(operation_window, index - menu.get_offset(), 0, "%s", menu.get_item(index).c_str());
            wattroff(operation_window, A_REVERSE);
        }
        else
        {
            mvwprintw(operation_window, index - menu.get_offset(), 0, "%s", menu.get_item(index).c_str());
        }
    }

    // NOTE: draw feedback window
    for (size_t index = 0; index < game.get_feedback_info().size(); index++)
    {
        mvwprintw(feedback_window, index, 0, "%s", game.get_feedback_info().at(index).c_str());
    }
}
