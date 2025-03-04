#include <ncurses.h>
#include <string>

int main(void)
{
    initscr();

    int win_line = 20;
    int win_col = 50;

    WINDOW *win = subwin(stdscr, win_line, win_col, (LINES - win_line) / 2, (COLS - win_col) / 2);

    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    std::string string = "Hello, World!";
    mvwprintw(win, win_line / 2, (win_col - string.length()) / 2, "%s", string.c_str());
    refresh();
    getch();

    endwin();
    return 0;
}
