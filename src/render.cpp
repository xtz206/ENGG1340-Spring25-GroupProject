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
}

void Renderer::draw_game(void)
{
    for (size_t line = 1; line < NODE_LINES - 1; line++)
    {
        mvwprintw(node_window, line, 1, "%s", std::string(NODE_COLS - 2, ' ').c_str());
    }

    if (game.select_city() != nullptr)
    {
        mvwprintw(node_window, 1, 1, "City: %s", game.select_city()->name.c_str());
        mvwprintw(node_window, 2, 1, "Hitpoint: %d", game.select_city()->hitpoint);
    }
    else
    {
        mvwprintw(node_window, 1, 1, "City: Unselected");
        mvwprintw(node_window, 2, 1, "Hitpoint: ");
    }

    for (size_t line = 0; line < game.get_map().size(); line++)
    {
        mvwprintw(map_window, line + 1, 1, "%s", game.get_map().at(line).c_str());
    }

    mvwprintw(map_window, game.get_cursor().first + 1, game.get_cursor().second + 1, "X");
}

void Renderer::draw_debug(const std::string &str)
{
    mvwprintw(operation_window, 8, 1, "%s", str.c_str());
}
