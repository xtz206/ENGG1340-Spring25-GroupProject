#include "render.h"
#include <fstream>
#include <ncurses.h>

#define TOTAL_LINES 40
#define TOTAL_COLS 120
#define MAP_LINES 28
#define MAP_COLS 90
#define RADAR_LINES 14
#define RADAR_COLS 30
#define NODE_LINES 14
#define NODE_COLS 30
#define INFO_LINES 12
#define INFO_COLS 30
#define OPERATION_LINES 12
#define OPERATION_COLS 90

Renderer::Renderer(void)
{
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

    mvwprintw(map_window, 0, 2, "Map");
    mvwprintw(radar_window, 0, 2, "Radar");
    mvwprintw(node_window, 0, 2, "Node");
    mvwprintw(info_window, 0, 2, "Info");
    mvwprintw(operation_window, 0, 2, "Operation");

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
}

void Renderer::render(void)
{
    wrefresh(map_window);
    wrefresh(radar_window);
    wrefresh(node_window);
    wrefresh(info_window);
    wrefresh(operation_window);
}

void Renderer::draw_map(void)
{
    std::ifstream file("background.txt");
    std::string line;
    int y = 1;
    while (std::getline(file, line))
    {
        mvwprintw(map_window, y++, 1, "%s", line.c_str());
    }
}