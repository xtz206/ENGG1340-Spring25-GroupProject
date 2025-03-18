#include "render.h"
#include <string>
#include <ncurses.h>

#define RENDER_DEBUG

#define TOTAL_LINES 24
#define TOTAL_COLS 60
#define MAP_LINES 16
#define MAP_COLS 40
#define RADAR_LINES 8
#define RADAR_COLS 20
#define INFO_LINES 8
#define INFO_COLS 20
#define OPERATION_LINES 8
#define OPERATION_COLS 60

#ifdef RENDER_DEBUG
void write_to_window(WINDOW *window, int lines, int cols, std::string message);
#endif


Renderer::Renderer(void)
{
    map_window = subwin(stdscr, MAP_LINES, MAP_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2);
    radar_window = subwin(stdscr, RADAR_LINES, RADAR_COLS, (LINES - TOTAL_LINES) / 2, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    info_window = subwin(stdscr, INFO_LINES, INFO_COLS, (LINES - TOTAL_LINES) / 2 + RADAR_LINES, (COLS - TOTAL_COLS) / 2 + MAP_COLS);
    operation_window = subwin(stdscr, OPERATION_LINES, OPERATION_COLS, (LINES - TOTAL_LINES) / 2 + MAP_LINES, (COLS - TOTAL_COLS) / 2);

    wborder(map_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(radar_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(info_window, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(operation_window, '|', '|', '-', '-', '+', '+', '+', '+');
}

void Renderer::render(void)
{
#ifdef RENDER_DEBUG
    write_to_window(map_window, MAP_LINES, MAP_COLS, "Map");
    write_to_window(radar_window, RADAR_LINES, RADAR_COLS, "Radar");
    write_to_window(info_window, INFO_LINES, INFO_COLS, "Info");
    write_to_window(operation_window, OPERATION_LINES, OPERATION_COLS, "Operation");
#endif
    wrefresh(map_window);
    wrefresh(radar_window);
    wrefresh(info_window);
    wrefresh(operation_window);
}

#ifdef RENDER_DEBUG
void write_to_window(WINDOW *window, int lines, int cols, std::string message)
{
    mvwprintw(window, lines / 2, cols / 2 - message.length() / 2, "%s", message.c_str());
}
#endif
