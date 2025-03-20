#include "render.h"
#include "control.h"
#include <fstream>
#include <string>
#include <ncurses.h>

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
    file.close();
}

void Renderer::draw_operation(std::vector<Button> &buttons)
{
    for (auto button : buttons)
    {
        draw_button(button);
    }
}

void Renderer::draw_radar(int count)
{
    int prev = 1 + (count - 1) % (RADAR_LINES - 2);
    int curr = 1 + (count) % (RADAR_LINES - 2);

    for (int col = 1; col < RADAR_COLS - 1; col++)
    {
        mvwaddch(radar_window, prev, col, ' ');
        wattron(radar_window, A_DIM);
        mvwaddch(radar_window, curr, col, '*');
        wattroff(radar_window, A_DIM);
    }
}

void Renderer::draw_debug(const std::string &str)
{
    mvwprintw(operation_window, 8, 1, "%s", str.c_str());
}

void Renderer::draw_button(const Button &button)
{
    mvwprintw(button.window, button.line, button.col, "%s", button.label.c_str());
}
