#include <ncurses.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "render.h"
#include "control.h"

int main(void)
{
    try
    {
        // ncurses initialization
        setlocale(LC_CTYPE, "");
        initscr();
        noecho();
        nodelay(stdscr, true);
        keypad(stdscr, true);
        mousemask(BUTTON1_CLICKED, NULL);
        curs_set(0);

        Renderer renderer;
        WINDOW *operation_window = renderer.get_operation_window();
        WINDOW *map_window = renderer.get_map_window();
        std::vector<Button> buttons = {
            Button(operation_window, 1, 1, 1, 13, "Build  Cruise"),
            Button(operation_window, 2, 1, 1, 13, "Build    IRBM"),
            Button(operation_window, 3, 1, 1, 13, "Launch Cruise"),
            Button(operation_window, 4, 1, 1, 13, "Launch   IRBM"),
            Button(operation_window, 5, 1, 1, 3, "Fix"),
            Button(map_window, 1, 1, 18, 98, "Map")};
        Controller controller(renderer, buttons);

        int count = 1;

        renderer.draw_map();
        while (true)
        {
            renderer.draw_operation(buttons);
            renderer.draw_radar(count);

            short key = getch();
            controller.handle_key(key);

            renderer.render();
            refresh();
            usleep(100000);
            count++;
        }

        endwin();
    }
    catch (const std::exception &e)
    {
        endwin();
        std::cerr << e.what() << '\n';
    }

    return 0;
}
