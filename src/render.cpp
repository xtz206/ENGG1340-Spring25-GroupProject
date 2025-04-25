#include <string>
#include <vector>
#include <fstream>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "render.h"

#define TOTAL_LINES 30
#define TOTAL_COLS 140
#define MAP_LINES 20
#define MAP_COLS 100
#define RADAR_LINES 10
#define RADAR_COLS 40
#define NODE_LINES 10
#define NODE_COLS 40
#define INFO_LINES 10
#define INFO_COLS 40
#define OPERATION_LINES 10
#define OPERATION_COLS 100
#define MENU_LINES 10
#define MENU_COLS 40
#define TECH_LINES 30
#define TECH_COLS 80

void Renderer::debug(const std::string &str, int line)
{
    mvwprintw(stdscr, line, 1, "%s", str.c_str());
}

void MenuRenderer::init(void)
{
    clear();

    menu_window = subwin(stdscr, MENU_LINES, MENU_COLS, (LINES - MENU_LINES) / 2, (COLS - MENU_COLS) / 2);
    box(menu_window, 0, 0);

    mvwprintw(menu_window, 0, (MENU_COLS - menu.get_title().length()) / 2, "%s", menu.get_title().c_str());
}

void MenuRenderer::render(void)
{
    wrefresh(menu_window);
}

void MenuRenderer::draw(void)
{
    for (size_t line = 1; line < MENU_LINES - 1; line++)
    {
        mvwprintw(menu_window, line, 1, "%s", std::string(MENU_COLS - 2, ' ').c_str());
    }

    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            wattron(menu_window, A_REVERSE);
            mvwprintw(menu_window, index + 1, (MENU_COLS - menu.get_items().at(index).length()) / 2, "%s", menu.get_items().at(index).c_str());
            wattroff(menu_window, A_REVERSE);
        }
        else
        {
            mvwprintw(menu_window, index + 1, (MENU_COLS - menu.get_items().at(index).length()) / 2, "%s", menu.get_items().at(index).c_str());
        }
        if (index == menu.get_items().size() - 1)
        {
            break;
        }
    }
}

void TechMenuRenderer::init(void)
{
    clear();

    menu_window = subwin(stdscr, TECH_LINES, TECH_COLS, (LINES - TECH_LINES) / 2, (COLS - TECH_COLS) / 2);
    box(menu_window, 0, 0);

    mvwprintw(menu_window, 0, (TECH_COLS - menu.get_title().length()) / 2, "%s", menu.get_title().c_str());
}

void TechMenuRenderer::render(void)
{
    wrefresh(menu_window);
}

void TechMenuRenderer::draw()
{
    for (size_t line = 1; line < TECH_LINES - 1; line++)
    {
        mvwprintw(menu_window, line, 1, "%s", std::string(TECH_COLS - 2, ' ').c_str());
    }
    mvwhline(menu_window, menu.get_items().size() + 1, 1, ACS_HLINE, TECH_COLS - 2);

    std::vector<std::string> items = menu.get_items();
    for (size_t index = 0; index < items.size(); index++)
    {
        if (index == menu.get_cursor())
        {
            wattron(menu_window, A_REVERSE);
            mvwprintw(menu_window, index + 1, 1, "%s", items.at(index).c_str());
            wattroff(menu_window, A_REVERSE);
        }
        else
        {
            mvwprintw(menu_window, index + 1, 1, "%s", items.at(index).c_str());
        }
        if (index == TECH_LINES - 1)
        {
            break;
        }
    }

    std::vector<std::string> description = menu.get_item_description();

    for (size_t index = 0; index < description.size(); index++)
    {
        mvwprintw(menu_window, index + menu.get_items().size() + 2, 1, "%s", description.at(index).c_str());
    }
}

void GameRenderer::init(void)
{
    clear();

    map_window = subwin(stdscr, MAP_LINES, MAP_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2);
    radar_window = subwin(stdscr, RADAR_LINES, RADAR_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    node_window = subwin(stdscr, NODE_LINES, NODE_COLS, (LINES - TOTAL_LINES) / 2 + RADAR_LINES, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    info_window = subwin(stdscr, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + RADAR_LINES + NODE_LINES, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    operation_window = subwin(stdscr, OPERATION_LINES, OPERATION_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES, (COLS - TOTAL_COLS) / 2);

    box(map_window, 0, 0);
    box(radar_window, 0, 0);
    box(node_window, 0, 0);
    box(info_window, 0, 0);
    box(operation_window, 0, 0);

    // TODO: store title in separate file instead of hardcoding
    mvwprintw(map_window, 0, 2, "Map");
    mvwprintw(radar_window, 0, 2, "Radar");
    mvwprintw(node_window, 0, 2, "Node");
    mvwprintw(info_window, 0, 2, "Info");
    mvwprintw(operation_window, 0, 2, "Operation");

    // TODO: store button text in separate file instead of hardcoding
    mvwprintw(operation_window, 1, 1, "W/A/S/D: CURSOR MOVING");
    mvwprintw(operation_window, 2, 1, "ENTER: PASS TURN");
    mvwprintw(operation_window, 3, 1, "ESC: PAUSE");
    mvwprintw(operation_window, 4, 1, "R: RESEARCH");
    mvwprintw(operation_window, 5, 1, "F: FIX");
    mvwprintw(operation_window, 6, 1, "B: BUILD");
    mvwprintw(operation_window, 7, 1, "L: LAUNCH");
    mvwprintw(operation_window, 8, 1, "Q: QUIT");
}

void GameRenderer::render(void)
{
    wrefresh(map_window);
    wrefresh(radar_window);
    wrefresh(node_window);
    wrefresh(info_window);
    wrefresh(operation_window);
}

void GameRenderer::draw(void)
{
    // RADAR WINDOW
    for (size_t line = 1; line < RADAR_LINES - 1; line++)
    {
        mvwprintw(radar_window, line, 1, "%s", std::string(RADAR_COLS - 2, ' ').c_str());
    }

    mvwprintw(radar_window, 1, 1, "Missile Num: %zu", game.get_missiles().size());
    mvwprintw(radar_window, 2, 1, "Researched: %zu", game.tech_tree.researched.size() - 1);
    mvwprintw(radar_window, 3, 1, "Available: %zu", game.tech_tree.available.size());

    // NODE WINDOW
    for (size_t line = 1; line < NODE_LINES - 1; line++)
    {
        mvwprintw(node_window, line, 1, "%s", std::string(NODE_COLS - 2, ' ').c_str());
    }

    if (game.is_selected_missile()) // DEBUG: en_enhanced_radar_III
    {
        AttackMissile &missile = static_cast<AttackMissile &>(game.select_missile());
        mvwprintw(node_window, 1, 1, "Target: %s", missile.city.name.c_str());
        mvwprintw(node_window, 2, 1, "Speed: %d", missile.speed);
        mvwprintw(node_window, 3, 1, "Damage: %d", missile.damage);
    }
    else if (game.is_selected_city())
    {
        City &city = game.select_city();
        mvwprintw(node_window, 1, 1, "Name: %s", city.name.c_str());
        mvwprintw(node_window, 2, 1, "Hitpoint: %d", city.hitpoint);
        mvwprintw(node_window, 3, 1, "Productivity: %d", city.productivity);
        mvwprintw(node_window, 4, 1, "Countdown: %d", city.countdown);
        if (true) // DEBUG: game.en_enhanced_radar_II
        {
            int missile_count = 0;
            for (auto missile : game.missile_manager.get_attack_missiles())
            {
                if (missile->get_target() == city.get_position())
                {
                    missile_count++;
                }
            }

            mvwprintw(node_window, 5, 1, "Targeted by %d Missiles", missile_count);
        }
    }
    else
    {
        mvwprintw(node_window, 1, 1, "Nothing Selected Now");
    }

    // INFO WINDOW
    for (size_t line = 1; line < INFO_LINES - 1; line++)
    {
        mvwprintw(info_window, line, 1, "%s", std::string(INFO_COLS - 2, ' ').c_str());
    }

    mvwprintw(info_window, 1, 1, "Turn: %d", game.get_turn());
    mvwprintw(info_window, 2, 1, "Deposit: %d", game.get_deposit());
    mvwprintw(info_window, 3, 1, "Productivity: %d", game.get_productivity());
    mvwprintw(info_window, 4, 1, "Researching: %s", game.tech_tree.researching != nullptr ? game.tech_tree.researching->name.c_str() : "None");
    mvwprintw(info_window, 5, 1, "Researching Time: %d", game.tech_tree.researching != nullptr ? game.tech_tree.remaining_time : 0);
    if (true) // DEBUG: game.en_enhanced_radar_I
    {
        mvwprintw(info_window, 6, 1, "Approaching Missile Count: %zu", game.get_missiles().size());
    }

    // MAP WINDOW
    for (size_t line = 0; line < game.get_background().size(); line++)
    {
        mvwprintw(map_window, line + 1, 1, "%s", game.get_background().at(line).c_str());
    }

    for (auto missile : game.get_missiles())
    {
        if (!game.is_in_map(missile->get_position()))
        {
            continue;
        }
        if (missile->get_progress() == MissileProgress::EXPLODED)
        {
            continue;
        }
        std::string direction = "";
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
        mvwprintw(map_window, missile->get_position().y + 1, missile->get_position().x + 1, "%s", direction.c_str());
    }

    mvwprintw(map_window, game.get_cursor().y + 1, game.get_cursor().x + 1, "X");
}
