#include <ncurses.h>
#include <string>

#include "render.h"

int main(void)
{
    initscr();

    Renderer renderer;
    renderer.render();

    refresh();
    getch();

    endwin();
    return 0;
}
