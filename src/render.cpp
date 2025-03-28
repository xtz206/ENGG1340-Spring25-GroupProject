#include <fstream>
#include <string>
#include <ncurses.h>

#include "game.h"
#include "render.h"
#include "control.h"

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

Renderer::Renderer(Game &g) : game(g)
{
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    curs_set(0);

    map_window = subwin(stdscr, MAP_LINES, MAP_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2);
    radar_window = subwin(stdscr, RADAR_LINES, RADAR_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    node_window = subwin(stdscr, NODE_LINES, NODE_COLS, (LINES - TOTAL_LINES) / 2 + RADAR_LINES, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    info_window = subwin(stdscr, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + RADAR_LINES + NODE_LINES, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    operation_window = subwin(stdscr, OPERATION_LINES, OPERATION_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES, (COLS - TOTAL_COLS) / 2);

    start_color();
}

void Renderer::render(void)
{
    wrefresh(map_window);
    wrefresh(radar_window);
    wrefresh(node_window);
    wrefresh(info_window);
    wrefresh(operation_window);
    refresh();
}

void Renderer::draw_window(void)
{
    box(map_window, 0, 0);
    box(radar_window, 0, 0);
    box(node_window, 0, 0);
    box(info_window, 0, 0);
    box(operation_window, 0, 0);

    mvwprintw(map_window, 0, 2, "Map");
    mvwprintw(radar_window, 0, 2, "Radar");
    mvwprintw(node_window, 0, 2, "Node");
    mvwprintw(info_window, 0, 2, "Info");
    mvwprintw(operation_window, 0, 2, "Operation");

    mvwprintw(operation_window, 1, 1, "w: Cursor Up");
    mvwprintw(operation_window, 2, 1, "s: Cursor Down");
    mvwprintw(operation_window, 3, 1, "a: Cursor Left");
    mvwprintw(operation_window, 4, 1, "d: Cursor Right");
    mvwprintw(operation_window, 5, 1, "c: Pass Turn");
    mvwprintw(operation_window, 6, 1, "f: Fix City");
    mvwprintw(operation_window, 7, 1, "q: Quit");
}

void Renderer::draw_game(void)
{
    // NODE WINDOW
    for (size_t line = 1; line < NODE_LINES - 1; line++)
    {
        mvwprintw(node_window, line, 1, "%s", std::string(NODE_COLS - 2, ' ').c_str());
    }

    if (game.select_city() != nullptr)
    {
        mvwprintw(node_window, 1, 1, "City: %s", game.select_city()->name.c_str());
        mvwprintw(node_window, 2, 1, "Hitpoint: %d", game.select_city()->hitpoint);
        mvwprintw(node_window, 3, 1, "Deposit: %d", game.select_city()->deposit);
        mvwprintw(node_window, 4, 1, "Productivity: %d", game.select_city()->productivity);
    }
    else
    {
        mvwprintw(node_window, 1, 1, "City: Unselected");
        mvwprintw(node_window, 2, 1, "Hitpoint: ");
        mvwprintw(node_window, 3, 1, "Deposit: ");
        mvwprintw(node_window, 4, 1, "Productivity: ");
    }

    // INFO WINDOW
    for (size_t line = 1; line < INFO_LINES - 1; line++)
    {
        mvwprintw(info_window, line, 1, "%s", std::string(INFO_COLS - 2, ' ').c_str());
    }

    mvwprintw(info_window, 1, 1, "Turn: %d", game.get_turn());

    // MAP WINDOW
    for (size_t line = 0; line < game.get_background().size(); line++)
    {
        mvwprintw(map_window, line + 1, 1, "%s", game.get_background().at(line).c_str());
    }

    for (auto &missile : game.missiles)
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

void Renderer::draw_debug(const std::string &str)
{
    mvwprintw(operation_window, 8, 1, "%s", str.c_str());
}
