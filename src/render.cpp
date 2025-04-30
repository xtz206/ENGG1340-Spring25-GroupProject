#include <string>
#include <vector>
#include <fstream>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "render.h"

// TODO: make screen size variable to map size
#define TOTAL_LINES 30
#define TOTAL_COLS 150
#define MAP_LINES 18
#define MAP_COLS 98
#define INFO_LINES 7
#define INFO_COLS 49
#define OPERATION_LINES 9
#define OPERATION_COLS 40
#define FEEDBACK_LINES 9
#define FEEDBACK_COLS 56
#define TUTORIAL_LINES 20
#define TUTORIAL_COLS 40
#define MENU_LINES 10
#define MENU_COLS 40
#define TECH_LINES 10
#define TECH_COLS 80

void Renderer::debug(const std::string &str, int line)
{
    mvwprintw(stdscr, line, 1, "%s", str.c_str());
}

void BasicMenuRenderer::init(void)
{
    erase();

    box_window = subwin(stdscr, MENU_LINES + 2, MENU_COLS + 2, (LINES - MENU_LINES - 2) / 2, (COLS - MENU_COLS - 2) / 2);
    item_window = subwin(box_window, MENU_LINES, MENU_COLS, (LINES - MENU_LINES) / 2, (COLS - MENU_COLS) / 2);
    box(box_window, 0, 0);
    mvwprintw(box_window, 0, (MENU_COLS - menu.get_title().length() + 2) / 2, "%s", menu.get_title().c_str());
}

void BasicMenuRenderer::render(void)
{
    wrefresh(item_window);
}

void BasicMenuRenderer::draw(void)
{
    werase(item_window);
    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            wattron(item_window, A_REVERSE);
            mvwprintw(item_window, index, (MENU_COLS - menu.get_item(index).length()) / 2, "%s", menu.get_item(index).c_str());
            wattroff(item_window, A_REVERSE);
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

    box_window = subwin(stdscr, TUTORIAL_LINES + MENU_LINES + 3, TUTORIAL_COLS + 2, (LINES - TUTORIAL_LINES - MENU_LINES - 3) / 2, (COLS - TUTORIAL_COLS - 2) / 2);
    page_window = subwin(box_window, TUTORIAL_LINES, TUTORIAL_COLS, (LINES - TUTORIAL_LINES - MENU_LINES - 3) / 2 + 1, (COLS - TUTORIAL_COLS) / 2);
    item_window = subwin(box_window, MENU_LINES, TUTORIAL_COLS, (LINES - TUTORIAL_LINES - MENU_LINES - 3) / 2 + TUTORIAL_LINES + 2, (COLS - TUTORIAL_COLS) / 2);

    box(box_window, 0, 0);
    mvwhline(box_window, TUTORIAL_LINES + 1, 1, ACS_HLINE, TUTORIAL_COLS);
    mvwaddch(box_window, TUTORIAL_LINES + 1, 0, ACS_LTEE);
    mvwaddch(box_window, TUTORIAL_LINES + 1, TUTORIAL_COLS + 1, ACS_RTEE);
    mvwprintw(box_window, 0, (TUTORIAL_COLS - menu.get_title().length() + 2) / 2, "%s", menu.get_title().c_str());
}

void TutorialMenuRenderer::render(void)
{
    wrefresh(page_window);
    wrefresh(item_window);
}

void TutorialMenuRenderer::draw(void)
{
    werase(page_window);
    werase(item_window);
    const std::vector<std::string> &page = menu.get_page();

    for (size_t index = 0; index < page.size(); index++)
    {
        if (index >= TUTORIAL_LINES)
        {
            break;
        }
        mvwprintw(page_window, index, (TUTORIAL_COLS - page.at(index).length()) / 2, "%s", page.at(index).c_str());
    }

    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index + 1 >= MENU_LINES)
        {
            break;
        }

        if (menu.get_cursor() == index)
        {
            wattron(item_window, A_REVERSE);
            mvwprintw(item_window, index, (MENU_COLS - menu.get_item(index).length()) / 2, "%s", menu.get_item(index).c_str());
            wattroff(item_window, A_REVERSE);
        }
        else
        {
            mvwprintw(item_window, index, (MENU_COLS - menu.get_item(index).length()) / 2, "%s", menu.get_item(index).c_str());
        }
    }

    std::string page_info = menu.get_page_info();
    mvwprintw(item_window, MENU_LINES - 1, (TUTORIAL_COLS - page_info.length()) / 2, "%s", page_info.c_str());
}

void TechMenuRenderer::init(void)
{
    erase();

    box_window = subwin(stdscr, TECH_LINES * 2 + 3, TECH_COLS + 2, (LINES - TECH_LINES * 2 - 2) / 2, (COLS - TECH_COLS - 2) / 2);
    item_window = subwin(box_window, TECH_LINES, TECH_COLS, (LINES - TECH_LINES * 2 - 3) / 2 + 1, (COLS - TECH_COLS) / 2);
    desc_window = subwin(box_window, TECH_LINES, TECH_COLS, (LINES - TECH_LINES * 2 - 3) / 2 + TECH_LINES + 2, (COLS - TECH_COLS) / 2);
    box(box_window, 0, 0);
    mvwprintw(box_window, 0, (TECH_COLS + 2 - menu.get_title().length()) / 2, "%s", menu.get_title().c_str());
    mvwhline(box_window, TECH_LINES + 1, 1, ACS_HLINE, TECH_COLS);
    mvwaddch(box_window, TECH_LINES + 1, 0, ACS_LTEE);
    mvwaddch(box_window, TECH_LINES + 1, TECH_COLS + 1, ACS_RTEE);
}

void TechMenuRenderer::render(void)
{
    wrefresh(item_window);
    wrefresh(desc_window);
}

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

void GameRenderer::init(void)
{
    erase();

    box_window = subwin(stdscr, TOTAL_LINES, TOTAL_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2);
    map_window = subwin(box_window, MAP_LINES, MAP_COLS, (LINES - TOTAL_LINES) / 2 + 1, (COLS - TOTAL_COLS) / 2 + 1);
    general_info_window = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + 1, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    selected_info_window = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + INFO_LINES + 2, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    tech_info_window = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + INFO_LINES * 2 + 3, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    super_weapon_info = subwin(box_window, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + INFO_LINES * 3 + 4, (COLS - TOTAL_COLS) / 2 + MAP_COLS + 2);
    operation_window = subwin(box_window, OPERATION_LINES, OPERATION_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES + 2, (COLS - TOTAL_COLS) / 2 + 1);
    feedback_window = subwin(box_window, FEEDBACK_LINES, FEEDBACK_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES + 2, (COLS - TOTAL_COLS) / 2 + OPERATION_COLS + 3);

    box(box_window, 0, 0);
    mvwhline(box_window, MAP_LINES + 1, 1, ACS_HLINE, MAP_COLS);
    mvwvline(box_window, 1, MAP_COLS + 1, ACS_VLINE, TOTAL_LINES - 2);

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
    mvwprintw(box_window, 0, 2, "Map");
    mvwprintw(box_window, 0, 2 + MAP_COLS + 2, "General");
    mvwprintw(box_window, INFO_LINES + 1, 2 + MAP_COLS + 2, "City & Missile");
    mvwprintw(box_window, INFO_LINES * 2 + 2, 2 + MAP_COLS + 2, "Technology & Research");
    mvwprintw(box_window, INFO_LINES * 3 + 3, 2 + MAP_COLS + 2, "Super Weapon");
    mvwprintw(box_window, MAP_LINES + 1, 2, "Operation");
    mvwprintw(box_window, MAP_LINES + 1, 2 + OPERATION_COLS + 2, "Feedback");
}

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

void GameRenderer::draw(void)
{
    werase(map_window);
    werase(general_info_window);
    werase(selected_info_window);
    werase(tech_info_window);
    werase(super_weapon_info);
    werase(operation_window);
    werase(feedback_window);

    // NOTE: draw map window
    for (int index = 0; index < game.get_background().size(); index++)
    {
        mvwprintw(map_window, index, 0, "%s", game.get_background().at(index).c_str());
    }

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
        case MissileDirection::A:
            direction = "O";
            break;
        case MissileDirection::N:
            direction = "↑";
            break;
        case MissileDirection::NE:
            direction = "↗";
            break;
        case MissileDirection::E:
            direction = "→";
            break;
        case MissileDirection::SE:
            direction = "↘";
            break;
        case MissileDirection::S:
            direction = "↓";
            break;
        case MissileDirection::SW:
            direction = "↙";
            break;
        case MissileDirection::W:
            direction = "←";
            break;
        case MissileDirection::NW:
            direction = "↖";
            break;
        case MissileDirection::U:
            direction = "x";
            break;
        default:
            break;
        }
        mvwprintw(map_window, missile->get_position().y, missile->get_position().x, "%s", direction.c_str());
    }
    mvwprintw(map_window, game.get_cursor().y, game.get_cursor().x, "X");

    // NOTE: draw info windows
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
