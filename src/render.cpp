/**
 * @file render.cpp
 * @brief This file contains the implementation of rendering functionalities for the game interface.
 * 
 * The file defines various classes and methods to manage and render different components of the game,
 * including windows, menus, and game states. It uses the ncurses library for terminal-based UI rendering.
 * 
 * Classes:
 * - Window: Represents a window in the terminal and provides methods to print text and manage attributes.
 * - Renderer: Provides debugging functionalities for rendering.
 * - BasicMenuRenderer: Handles rendering of basic menus with items and titles.
 * - TitleMenuRenderer: Manages rendering of title menus with left-aligned and centered items.
 * - SaveMenuRenderer: Renders the save menu interface with warnings.
 * - EndMenuRenderer: Displays the end menu with game results and statistics.
 * - TutorialMenuRenderer: Renders a tutorial menu with pages and items.
 * - TechMenuRenderer: Handles rendering of technology menus with descriptions.
 * - GameRenderer: Manages and renders the entire game interface, including map, information, and operations.
 * 
 * Dependencies:
 * - ncurses library for terminal-based UI rendering.
 * - Various game-related classes such as Game, Menu, and Position.
 * 
 * Key Features:
 * - Flexible window management with subwindows.
 * - Support for printing text with attributes (e.g., bold, color).
 * - Rendering of menus with alignment options (left, center, right).
 * - Dynamic rendering of game states, including map, statistics, and feedback.
 * - Integration with game logic for displaying real-time updates.
 */

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "render.h"

#define ALL_SIZE Size(LINES, COLS)

/**
* @brief Constructor for the Window class.
*
* @param win The parent window.
* @param s The size of the window.
* @param p The position of the window.
*
*/

Window::Window(WINDOW *win, Size s, Position p) : size(s), pos(p)
{
    window = subwin(win, s.h, s.w, p.y, p.x);
}

/**
 * @brief Constructor for the Window class.
 * 
 * @param win The parent window.
 * @param s The size of the window.
 * @param p The position of the window.
 * 
 */

Window::Window(Window &win, Size s, Position p) : size(s), pos(p)
{
    window = subwin(win.window, s.h, s.w, p.y, p.x);
}

/**
 * @brief Prints a character with specified attributes at a given position within the window.
 * 
 * This function places a character (`ch`) at the specified position (`p`) in the window,
 * applying the given attributes (`attr`). If the position is outside the bounds of the
 * window, the function returns without performing any action.
 * 
 * @param p The position (x, y) where the character should be printed. 
 *          It is represented as a `Position` object.
 * @param ch The character to be printed, represented as a `chtype`.
 * @param attr The attributes to be applied to the character, represented as an `attr_t`.
 *             Attributes can include color, bold, underline, etc.
 */
void Window::print(Position p, chtype ch, attr_t attr)
{
    if (p.y >= size.h || p.x >= size.w)
    {
        return;
    }
    wattron(window, attr);
    mvwaddch(window, p.y, p.x, ch);
    wattroff(window, attr);
}

/**
 * @brief Prints a string with specified attributes at a given position within the window.
 * 
 * This function places a string (`s`) at the specified position (`p`) in the window,
 * applying the given attributes (`attr`). If the position is outside the bounds of the
 * window, the function returns without performing any action.
 * 
 * @param p The position (x, y) where the string should be printed. 
 *          It is represented as a `Position` object.
 * @param s The string to be printed, represented as a `const char*`.
 * @param attr The attributes to be applied to the string, represented as an `attr_t`.
 *             Attributes can include color, bold, underline, etc.
 */
void Window::print(Position p, const char *s, attr_t attr)
{
    if (p.y >= size.h || p.x >= size.w)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, p.y, p.x, "%s", s);
    wattroff(window, attr);
}


/**
 * @brief Prints a line of spaces with the specified attributes in a window.
 * 
 * This function fills an entire line of the window with spaces, starting from
 * the beginning of the line (column 0) and extending to the width of the window.
 * The spaces are printed with the specified text attributes.
 * 
 * @param line The line number (row) in the window where the spaces will be printed.
 *             If the line number is greater than or equal to the height of the window,
 *             the function returns without performing any action.
 * @param attr The text attributes to be applied to the spaces (e.g., color, bold).
 *             These attributes are applied using `wattron` and removed using `wattroff`.
 */
void Window::print_spaces(int line, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, line, 0, "%s", std::string(size.w, ' ').c_str());
    wattroff(window, attr);
}

/**
 * @brief Prints a string aligned to the left at a specified line in the window.
 * 
 * This function displays a string `s` starting from the leftmost position of the specified
 * line in the window. If the string exceeds the width of the window, it is truncated to fit.
 * The specified attributes `attr` are applied to the string while rendering.
 * 
 * @param line The line number (0-based index) where the string will be printed.
 *             If the line number exceeds the height of the window, the function returns without printing.
 * @param s The string to be printed.
 * @param attr The attributes to be applied to the string (e.g., color, bold, etc.).
 */
void Window::print_left(int line, const std::string &s, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, line, 0, "%s", s.c_str());
    if (s.length() > size.w)
    {
        mvwprintw(window, line, 0, "%s", s.substr(0, size.w - 1).c_str());
    }
    else
    {
        mvwprintw(window, line, 0, "%s", s.c_str());
    }
    wattroff(window, attr);
}

/**
 * @brief Prints a string centered on a specified line within the window.
 *
 * This function centers a given string on a specified line of the window.
 * If the string is longer than the window's width, it truncates the string
 * to fit within the window. The text is displayed with the specified
 * attributes.
 *
 * @param line The line number (0-based index) where the string will be printed.
 *             If the line number exceeds the window's height, the function
 *             returns without performing any action.
 * @param s The string to be printed.
 * @param attr The text attributes to be applied (e.g., bold, underline).
 */
void Window::print_center(int line, const std::string &s, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, line, (size.w - s.length()) / 2, "%s", s.c_str());
    if (s.length() > size.w)
    {
        mvwprintw(window, line, 0, "%s", s.substr(0, size.w - 1).c_str());
    }
    else
    {
        mvwprintw(window, line, (size.w - s.length()) / 2, "%s", s.c_str());
    }
    wattroff(window, attr);
}

/**
 * @brief Prints a string aligned to the right on a specified line within the window.
 * 
 * This function displays a string on a specified line of the window, aligning it to the right.
 * If the string is longer than the window's width, it will be truncated to fit.
 * The specified attributes are applied to the text during rendering.
 * 
 * @param line The line number (0-based index) where the string will be printed.
 *             If the line number exceeds the window's height, the function returns without action.
 * @param s The string to be printed. If its length exceeds the window's width, it will be truncated.
 * @param attr The text attributes to be applied (e.g., color, bold, etc.).
 */
void Window::print_right(int line, const std::string &s, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }

    wattron(window, attr);
    if (s.length() > size.w)
    {
        mvwprintw(window, line, 0, "%s", s.substr(0, size.w - 1).c_str());
    }
    else
    {
        mvwprintw(window, line, size.w - s.length(), "%s", s.c_str());
    }
    mvwprintw(window, line, size.w - s.length(), "%s", s.c_str());
    wattroff(window, attr);
}

/**
 * @brief Displays a debug message on the screen at a specified line.
 * 
 * This function uses the `mvwprintw` function to print a string to the 
 * standard screen (`stdscr`) at the specified line number. It is useful 
 * for debugging purposes to display messages during program execution.
 * 
 * @param str The debug message to be displayed.
 * @param line The line number on the screen where the message will be printed.
 */
void Renderer::debug(const std::string &str, int line)
{
    mvwprintw(stdscr, line, 1, "%s", str.c_str());
}


/**
 * @brief Constructs a BasicMenuRenderer object to render a menu with specified size.
 * 
 * @param m Reference to the Menu object to be rendered.
 * @param s Size of the menu to be displayed.
 * 
 * This constructor initializes the renderer by calculating the position of the menu
 * on the screen, creating a bordered window for the menu, and setting up an inner
 * window for rendering menu items.
 */
BasicMenuRenderer::BasicMenuRenderer(Menu &m, Size s)
    : menu(m), size(s), pos((ALL_SIZE - s - Size(2, 2)) / 2),
      box_window(Window(stdscr, s + Size(2, 2), pos)),
      item_window(Window(box_window, s, pos + Size(1, 1)))
{
}

/**
 * @brief Initializes the basic menu renderer by clearing the screen, 
 *        drawing the window margin, and displaying the menu title at the center.
 */
 
void BasicMenuRenderer::init(void)
{
    erase();
    box_window.draw_margin();
    box_window.print_center(0, menu.get_title());
}

/**
 * @brief Renders the basic menu by refreshing the item window.
 */
void BasicMenuRenderer::render(void)
{
    item_window.refresh(); // Update previous content
}

/**
 * @brief Renders a basic menu interface by drawing menu items on the screen.
 * 
 * This function clears the item window and iterates through the menu items.
 * It highlights the item at the current cursor position and displays all
 * items centered in the window.
 */
void BasicMenuRenderer::draw(void)
{
    item_window.erase();
    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }
}

/**
 * @brief Constructs a VideoRenderer object to render a title video within a specified size and position.
 * 
 * @param m Reference to a TitleVideo object representing the video to be rendered.
 * @param s Size object specifying the dimensions of the rendering area.
 * 
 * This constructor initializes the VideoRenderer with the given TitleVideo and size.
 * It calculates the position of the rendering area to center it within the available space.
 * A Window object is created to represent the rendering area on the screen.
 */
VideoRenderer::VideoRenderer(TitleVideo &m, Size s)
    : menu(m), size(s), pos((ALL_SIZE - s) / 2),
      video_window(Window(stdscr, s, pos))
{
}

/**
 * @brief Initializes the video renderer by performing necessary setup operations.
 * 
 * This function is responsible for preparing the video renderer for use. 
 * It clears any existing state or data to ensure a clean starting point.
 */
void VideoRenderer::init(void)
{
    erase();
}

/**
 * @brief Renders the video content by refreshing the video window.
 * 
 * This function is responsible for updating the video display by calling
 * the `refresh` method on the `video_window` object. It ensures that the
 * video content is rendered properly on the screen.
 */
void VideoRenderer::render(void)
{
    video_window.refresh();
}

/**
 * @brief Renders the current frame of the video onto the video window.
 * 
 * This function clears the video window and then retrieves the current frame
 * from the menu object. It iterates through the lines of the frame and prints
 * them to the video window, ensuring that the number of lines does not exceed
 * the height of the video window.
 * 
 * @note The function assumes that the `menu` object provides a valid frame
 *       and that the `video_window` object supports the `erase` and 
 *       `print_left` methods.
 */
void VideoRenderer::draw(void)
{
    video_window.erase();

    const std::vector<std::string> &frame = menu.get_frame();
    for (size_t index = 0; index < frame.size(); index++)
    {
        if (index >= size.h)
        {
            break;
        }
        video_window.print_center(index, frame.at(index));
    }
}

/**
 * @brief Constructs a TitleMenuRenderer object to render a title menu.
 * 
 * @param m Reference to the TitleMenu object to be rendered.
 * @param s Size of the rendering area.
 */
TitleMenuRenderer::TitleMenuRenderer(TitleMenu &m, Size s)
    : menu(m), size(s), pos((ALL_SIZE - s) / 2),
      title_window(Window(stdscr, s, pos))
{
}

/**
 * @brief Initializes the title menu renderer by clearing the screen and rendering
 *        the menu items on the title window.
 * 
 * This function erases the current screen content, then iterates through the menu
 * items to display them on the title window. All items except the last one are 
 * printed left-aligned, while the last item is printed centered with a reverse 
 * attribute for emphasis.
 */
void TitleMenuRenderer::init(void)
{
    erase();

    for (size_t index = 0; index < menu.get_items().size() - 1; index++)
    {
        title_window.print_left(index, menu.get_item(index));
    }

    title_window.print_center(size.h - 1, menu.get_item(menu.get_items().size() - 1), A_REVERSE);
}

void TitleMenuRenderer::render(void)
{
    title_window.refresh();
}

void TitleMenuRenderer::draw(void)
{
}

/**
 * @brief Renders the save menu interface.
 * 
 * This function clears the item window and iterates through the menu items to display them.
 * The currently selected menu item is highlighted. A warning message is displayed at the 
 * bottom of the window to inform the user about the consequences of saving on full.
 */
void SaveMenuRenderer::draw(void)
{
    item_window.erase();
    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }

    item_window.print_center(size.h - 2, "SAVE ON FULL WILL OVERWRITE", COLOR_PAIR(3));
}

/**
 * @brief Constructs an EndMenuRenderer object to render the end menu of the game.
 * 
 * @param g Reference to the Game object, representing the current game state.
 * @param m Reference to the Menu object, representing the menu to be rendered.
 * @param ds Size object representing the dimensions of the description window.
 * @param is Size object representing the dimensions of the item window.
 */
EndMenuRenderer::EndMenuRenderer(Game &g, Menu &m, Size ds, Size is)
    : menu(m), game(g), desc_size(ds), item_size(is), pos((ALL_SIZE - Size(is.h + ds.h + 2, is.w + 2)) / 2),
      box_window(Window(stdscr, Size(is.h + ds.h + 3, is.w + 2), pos)),
      desc_window(Window(box_window, desc_size, pos + Size(1, 1))),
      item_window(Window(box_window, item_size, pos + Size(ds.h + 2, 1)))
{
}

/**
 * @brief Initializes and renders the end menu screen with game results.
 * 
 * This function clears the screen, draws the menu layout, and displays
 * the game results including win/lose status, score, casualty count, 
 * and the number of turns taken. It formats the score and casualty 
 * values based on their magnitude for better readability.
 */
void EndMenuRenderer::init(void)
{
    erase();

    box_window.draw_margin();
    box_window.print_center(0, menu.get_title());
    box_window.draw_hline(Size(desc_size.h + 1, 1), item_size.w);
    box_window.draw_char(Size(desc_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Size(desc_size.h + 1, item_size.w + 1), ACS_RTEE);

    std::ostringstream oss;

    if (game.get_enemy_hp() > 0)
    {
        desc_window.print_spaces(0, COLOR_PAIR(2));
        desc_window.print_center(0, "YOU LOSE", COLOR_PAIR(2));
    }
    else
    {
        desc_window.print_spaces(0, COLOR_PAIR(4));
        desc_window.print_center(0, "YOU  WIN", COLOR_PAIR(4));
    }

    desc_window.print_left(1, "Score:", A_NORMAL);
    if (game.get_score() > 1000)
    {
        oss << std::fixed << std::setprecision(2) << static_cast<double>(game.get_score()) / 1000 << "K";
        desc_window.print_right(1, oss.str(), COLOR_PAIR(4));
    }
    else if (game.get_score() > 100)
    {
        oss << game.get_score();
        desc_window.print_right(1, oss.str(), COLOR_PAIR(3));
    }
    else
    {
        oss << game.get_score();
        desc_window.print_right(1, oss.str(), COLOR_PAIR(2));
    }
    oss.str("");

    desc_window.print_left(2, "Casualty:", A_NORMAL);
    if (game.get_casualty() > 1000)
    {
        oss << std::fixed << std::setprecision(2) << static_cast<double>(game.get_casualty()) / 1000 << "M";
        desc_window.print_right(2, oss.str(), COLOR_PAIR(2));
    }
    else if (game.get_casualty() > 100)
    {
        oss << game.get_casualty() << "K";
        desc_window.print_right(2, oss.str(), COLOR_PAIR(3));
    }
    else
    {
        oss << game.get_casualty() << "K";
        desc_window.print_right(2, oss.str(), COLOR_PAIR(4));
    }
    oss.str("");

    desc_window.print_left(3, "Turn:", A_NORMAL);
    desc_window.print_right(3, std::to_string(game.get_turn()), A_NORMAL);
}

void EndMenuRenderer::render(void)
{
    item_window.refresh();
}

/**
 * @brief Renders the end menu by drawing menu items on the screen.
 * 
 * This function clears the item window and iterates through the menu items.
 * It highlights the item at the current cursor position and displays all
 * items centered in the window.
 */
void EndMenuRenderer::draw(void)
{
    item_window.erase();

    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }
}

/**
 * @brief Constructs a TutorialMenuRenderer object to render a tutorial menu.
 * 
 * @param m Reference to the TutorialMenu object to be rendered.
 * @param ps Size of the page window within the menu.
 * @param is Size of the item window within the menu.
 * 
 * Initializes the renderer's position, box window, page window, and item window
 * based on the provided sizes and menu layout.
 */
TutorialMenuRenderer::TutorialMenuRenderer(TutorialMenu &m, Size ps, Size is)
    : menu(m), page_size(ps), item_size(is), pos((ALL_SIZE - Size(ps.h + is.h + 3, ps.w + 2)) / 2),
      box_window(stdscr, Size(ps.h + is.h + 3, ps.w + 2), pos),
      page_window(box_window, ps, pos + Size(1, 1)),
      item_window(box_window, is, pos + Size(ps.y + 2, 1))
{
}

/**
 * @brief Initializes the tutorial menu renderer by setting up the display.
 * 
 * This function clears the current screen, draws the margin and horizontal 
 * line for the menu box, and places the title of the menu at the center 
 * of the top row.
 */
void TutorialMenuRenderer::init(void)
{
    erase();
    box_window.draw_margin();
    box_window.draw_hline(Size(page_size.h + 1, 1), page_size.w);
    box_window.draw_char(Size(page_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Size(page_size.h + 1, page_size.w + 1), ACS_RTEE);
    box_window.print_center(0, menu.get_title());
}

void TutorialMenuRenderer::render(void)
{
    page_window.refresh();
    item_window.refresh();
}

/**
 * @brief Renders the tutorial menu on the screen by drawing the page content
 *        and menu items with appropriate formatting.
 * 
 * This function clears the page and item windows, then iterates through the
 * menu's page content and items to display them. The currently selected menu
 * item is highlighted using reverse video formatting. Additionally, it displays
 * page information at the bottom of the item window.
 */
void TutorialMenuRenderer::draw(void)
{
    page_window.erase();
    item_window.erase();

    const std::vector<std::string> &page = menu.get_page();
    for (size_t index = 0; index < page.size(); index++)
    {
        page_window.print_center(index, page.at(index));
    }

    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }
    item_window.print_center(item_size.h - 1, menu.get_page_info());
}

/**
 * @brief Constructs a TechMenuRenderer object to render a technology menu.
 * 
 * @param m Reference to the TechMenu object to be rendered.
 * @param is Size of the menu items to be displayed.
 * @param ds Size of the description area for the menu.
 * 
 * This constructor initializes the renderer with the given menu, item size, 
 * and description size. It calculates the position of the rendering area 
 * and sets up the necessary windows for rendering the menu, items, and 
 * descriptions within a bordered box.
 */
TechMenuRenderer::TechMenuRenderer(TechMenu &m, Size is, Size ds)
    : menu(m), item_size(is), desc_size(ds), pos((ALL_SIZE - Size(is.h + ds.h + 3, is.w + 2)) / 2),
      box_window(stdscr, Size(is.h + ds.h + 3, is.w + 2), pos),
      item_window(box_window, is, pos + Size(1, 1)),
      desc_window(box_window, ds, pos + Size(is.y + 2, 1))
{
}

/**
 * @brief Initializes the TechMenuRenderer by setting up the menu display.
 * 
 * This function clears the screen, draws the margin for the menu box, 
 * centers and prints the menu title, and draws horizontal and vertical 
 * lines to separate the menu items visually.
 */
void TechMenuRenderer::init(void)
{
    erase();
    box_window.draw_margin();
    box_window.print_center(0, menu.get_title());
    box_window.draw_hline(Size(item_size.h + 1, 1), item_size.w);
    box_window.draw_char(Size(item_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Size(item_size.h + 1, item_size.w + 1), ACS_RTEE);
}

/// @brief Refresh tech menu display
void TechMenuRenderer::render(void)
{
    item_window.refresh();
    desc_window.refresh();
}

/**
 * @brief Renders the technical menu interface by drawing the menu items and their descriptions.
 * 
 * This function clears the item and description windows, then iterates through the menu items
 * to display them. The currently selected item is highlighted. Additionally, it displays the 
 * description of the selected item in the description window.
 */
void TechMenuRenderer::draw(void)
{
    item_window.erase();
    desc_window.erase();

    for (int index = 0; index < menu.get_limit(); index++)
    {
        if (index + menu.get_offset() == menu.get_cursor())
        {
            item_window.print_left(index, menu.get_item(index + menu.get_offset()), A_REVERSE);
        }
        else
        {
            item_window.print_left(index, menu.get_item(index + menu.get_offset()));
        }
    }

    std::vector<std::string> description = menu.get_item_description();
    for (size_t index = 0; index < description.size(); index++)
    {
        desc_window.print_left(index, description.at(index));
    }
}

/**
 * @brief Constructs a GameRenderer object to manage and render the game interface.
 * 
 * @param g Reference to the Game object containing game state and logic.
 * @param m Reference to the OperationMenu object for managing user operations.
 * @param s Size object representing the dimensions of certain UI components.
 * @param fs Vector of integers specifying the heights of various information windows.
 */
GameRenderer::GameRenderer(Game &g, OperationMenu &m, Size s, const std::vector<int> &fs)
    : game(g), menu(m), fields(fs),
      map_size(game.get_size()), info_size(map_size.h + s.h + 1, s.w),
      operation_size(s.h, map_size.w / 3),
      feedback_size(s.h, map_size.w - map_size.w / 3 - 1),
      pos((ALL_SIZE - map_size - s - Size(3, 3)) / 2),
      box_window(stdscr, map_size + s + Size(3, 3), pos),
      map_window(box_window, map_size, pos + Size(1, 1)),
      info_window(box_window, info_size, pos + Size(1, map_size.x + 2)),
      general_info_window(box_window, Size(fs.at(0), info_size.w), pos + Size(1, map_size.x + 2)),
      selected_info_window(box_window, Size(fs.at(1), info_size.w), pos + Size(fs.at(0) + 2, map_size.x + 2)),
      tech_info_window(box_window, Size(fs.at(2), info_size.w), pos + Size(fs.at(0) + fs.at(1) + 3, map_size.x + 2)),
      super_weapon_info_window(box_window, Size(fs.at(3), info_size.w), pos + Size(fs.at(0) + fs.at(1) + fs.at(2) + 4, map_size.x + 2)),
      operation_window(box_window, operation_size, pos + Size(map_size.y + 2, 1)),
      feedback_window(box_window, feedback_size, pos + Size(map_size.y + 2, operation_size.x + 2))
{
}

/**
 * @brief Initializes the game renderer by setting up the layout and drawing the UI components.
 * 
 * This function erases the current screen and draws the necessary UI elements, including margins, 
 * horizontal and vertical lines, and various labeled sections for the game interface. It also 
 * places specific characters at intersections and prints labels for different sections of the UI.
 * 
 * The layout is divided into the following sections:
 * - Map
 * - General Information
 * - City & Missile
 * - Technology & Research
 * - Super Weapon
 * - Operation and Feedback
 * 
 * The function uses the `box_window` object to draw the UI components and relies on the dimensions 
 * provided by `map_size`, `info_size`, `operation_size`, and `fields` to determine the positions 
 * of the elements.
 * 
 * @note The function assumes that the `fields` vector contains at least three elements, which 
 *       represent the heights of the "City & Missile", "Technology & Research", and "Super Weapon" 
 *       sections, respectively.
 */
void GameRenderer::init(void)
{
    erase();

    box_window.draw_margin();
    box_window.draw_hline(Position(map_size.h + 1, 1), map_size.w);
    box_window.draw_vline(Position(1, map_size.w + 1), info_size.h);
    box_window.draw_vline(Position(map_size.h + 2, operation_size.w + 1), operation_size.h);
    box_window.draw_hline(Position(fields.at(0) + 1, map_size.w + 2), info_size.w);
    box_window.draw_hline(Position(fields.at(0) + fields.at(1) + 2, map_size.w + 2), info_size.w);
    box_window.draw_hline(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 2), info_size.w);

    box_window.draw_char(Position(map_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Position(map_size.h + 1, map_size.w + 1), ACS_RTEE);
    box_window.draw_char(Position(0, map_size.w + 1), ACS_TTEE);
    box_window.draw_char(Position(info_size.h + 1, map_size.w + 1), ACS_BTEE);
    box_window.draw_char(Position(map_size.h + 1, operation_size.w + 1), ACS_TTEE);
    box_window.draw_char(Position(map_size.h + operation_size.h + 2, operation_size.w + 1), ACS_BTEE);

    box_window.draw_char(Position(fields.at(0) + 1, map_size.w + 1), ACS_LTEE);
    box_window.draw_char(Position(fields.at(0) + 1, map_size.w + info_size.w + 2), ACS_RTEE);
    box_window.draw_char(Position(fields.at(0) + fields.at(1) + 2, map_size.w + 1), ACS_LTEE);
    box_window.draw_char(Position(fields.at(0) + fields.at(1) + 2, map_size.w + info_size.w + 2), ACS_RTEE);
    if (fields.at(0) + fields.at(1) + fields.at(2) + 3 == map_size.h + 1)
    {
        box_window.draw_char(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 1), ACS_PLUS);
    }
    else
    {
        box_window.draw_char(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 1), ACS_LTEE);
    }
    box_window.draw_char(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + info_size.w + 2), ACS_RTEE);

    box_window.print(Position(0, 2), "Map");
    box_window.print(Position(0, map_size.w + 3), "General");
    box_window.print(Position(fields.at(0) + 1, map_size.w + 3), "City & Missile");
    box_window.print(Position(fields.at(0) + fields.at(1) + 2, map_size.w + 3), "Technology & Research");
    box_window.print(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 3), "Super Weapon");
    box_window.print(Position(map_size.h + 1, 2), "Operation Q/E/ENTER");
    box_window.print(Position(map_size.h + 1, operation_size.w + 3), "Feedback");
}

/**
 * @brief Renders the game interface by refreshing all the relevant windows.
 * 
 * This function updates the display of various game interface components,
 * ensuring that all windows are redrawn with the latest information.
 * It is responsible for refreshing the following windows:
 * - Map window
 * - General information window
 * - Selected information window
 * - Technology information window
 * - Super weapon information window
 * - Operation window
 * - Feedback window
 */
void GameRenderer::render(void)
{
    map_window.refresh();
    general_info_window.refresh();
    selected_info_window.refresh();
    tech_info_window.refresh();
    super_weapon_info_window.refresh();
    operation_window.refresh();
    feedback_window.refresh();
    // refresh();
}

/**
 * @brief Renders the entire game interface, including map, general information, 
 *        selected object information, technology tree, super weapon status, 
 *        operation menu, and feedback messages.
 * 
 * This function is responsible for clearing and redrawing all game windows 
 * to reflect the current state of the game. It handles the following:
 * 
 * - Map Window: Displays the game map, including terrain, missiles, and cursor.
 * - General Info Window: Shows game statistics such as turn, deposit, productivity, 
 *   enemy HP, and system statuses.
 * - Selected Info Window: Displays detailed information about the currently 
 *   selected missile or city.
 * - Tech Info Window: Shows the current research status, available technologies, 
 *   and researched technologies.
 * - Super Weapon Info Window: Displays the status of super weapons, including 
 *   readiness and remaining counters.
 * - Operation Window: Displays the operation menu with selectable options.
 * - Feedback Window: Shows recent feedback messages from the game.
 * 
 * The function ensures that all visual elements are updated to match the 
 * current game state, providing a seamless user interface.
 */

/// @brief Update all game interface elements
void GameRenderer::draw(void)
{   
    // Clear all subwindows
    map_window.erase();
    general_info_window.erase();
    selected_info_window.erase();
    tech_info_window.erase();
    super_weapon_info_window.erase();
    operation_window.erase();
    feedback_window.erase();

    // NOTE: draw map window
    for (int line = 0; line < game.get_size().h; line++)
    {
        for (int col = 0; col < game.get_size().w; col++)
        {
            char ch = game.get_background().at(line).at(col);
            switch (ch)
            {
            case ' ':
                map_window.print(Position(line, col), " ", COLOR_PAIR(0));
                break;
            case '@':
                map_window.print(Position(line, col), "@", COLOR_PAIR(3));
                break;
            case '#':
                map_window.print(Position(line, col), " ", COLOR_PAIR(1));
                break;
            }
        }
    }

    // Active missiles
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
        case MissileDirection::A://hit the object
            direction = "O";
            break;
        case MissileDirection::N: // North
            direction = "↑ ";
            break;
        case MissileDirection::NE:// Northeast
            direction = "↗ ";
            break;
        case MissileDirection::E://East
            direction = "→ ";
            break;
        case MissileDirection::SE://Southeast
            direction = "↘ ";
            break;
        case MissileDirection::S://South
            direction = "↓ ";
            break;
        case MissileDirection::SW://Southwest
            direction = "↙ ";
            break;
        case MissileDirection::W://West
            direction = "← ";
            break;
        case MissileDirection::NW:
            direction = "↖ ";
            break;
        default:
            break;
        }

        map_window.print(missile->get_position(), direction, COLOR_PAIR(missile->get_type() == MissileType::ATTACK ? 2 : 4));
    }
    int color_pair = (game.is_on_land(game.get_cursor()) ? 0 : (game.is_on_sea(game.get_cursor()) ? 1 : 3));
    map_window.print(game.get_cursor(), "*", COLOR_PAIR(color_pair));

    // NOTE: draw general windows
    general_info_window.print_left(0, "Turn:", A_NORMAL);
    general_info_window.print_right(0, std::to_string(game.get_turn()), A_NORMAL);
    general_info_window.print_left(1, "Deposit:", A_NORMAL);
    general_info_window.print_right(1, std::to_string(game.get_deposit()), A_NORMAL);
    general_info_window.print_left(2, "Productivity:", A_NORMAL);
    general_info_window.print_right(2, std::to_string(game.get_productivity()), A_NORMAL);
    general_info_window.print_left(3, "Enemy HP:", A_NORMAL);
    general_info_window.print_right(3, std::to_string(game.get_enemy_hp()), A_NORMAL);
    if (game.en_self_defense_sys)
    {
        general_info_window.print_left(4, "Self Defense System:", A_NORMAL);
        general_info_window.print_right(4, "ON", A_NORMAL);
    }
    if (game.en_enhanced_radar_I)
    {
        int missile_count = game.missile_manager.get_attack_missiles().size();
        if (missile_count == 0)
        {
            general_info_window.print_spaces(5, COLOR_PAIR(4));
            general_info_window.print_left(5, "No Missiles Approaching", COLOR_PAIR(4));
        }
        else if (missile_count < 5)
        {
            general_info_window.print_spaces(5, COLOR_PAIR(3));
            general_info_window.print_left(5, std::to_string(missile_count) + " Missile Approaching", COLOR_PAIR(3));
        }
        else
        {
            general_info_window.print_spaces(5, COLOR_PAIR(2));
            general_info_window.print_left(5, std::to_string(missile_count) + " Missiles Approaching !!!", COLOR_PAIR(2));
        }
    }

    // NOTE: draw selected info window
    if (game.is_selected_missile() && game.en_enhanced_radar_III)
    {
        AttackMissile &missile = dynamic_cast<AttackMissile &>(game.select_missile());
        selected_info_window.print_left(0, "Target:", A_NORMAL);
        selected_info_window.print_left(1, "Speed:", A_NORMAL);
        selected_info_window.print_left(2, "Damage:", A_NORMAL);

        selected_info_window.print_right(0, missile.city.name, A_NORMAL);
        selected_info_window.print_right(1, std::to_string(missile.speed), missile.speed > 2 ? COLOR_PAIR(2) : COLOR_PAIR(3));
        selected_info_window.print_right(2, std::to_string(missile.damage), missile.damage > 200 ? COLOR_PAIR(2) : COLOR_PAIR(3));
    }
    else if (game.is_selected_city())
    {
        City &city = game.select_city();

        selected_info_window.print_left(0, "Name:", A_NORMAL);
        selected_info_window.print_left(1, "Hitpoint:", A_NORMAL);
        selected_info_window.print_left(2, "Productivity:", A_NORMAL);
        selected_info_window.print_left(3, "Countdown:", A_NORMAL);
        selected_info_window.print_left(4, "Cruise Storage:", A_NORMAL);

        selected_info_window.print_right(0, city.name, A_NORMAL);
        selected_info_window.print_right(1, std::to_string(city.hitpoint), A_NORMAL);
        selected_info_window.print_right(2, std::to_string(city.productivity), A_NORMAL);
        selected_info_window.print_right(3, std::to_string(city.countdown), A_NORMAL);
        selected_info_window.print_right(4, std::to_string(city.cruise_storage), A_NORMAL);

        if (game.en_enhanced_radar_II)
        {
            int missile_count = 0;
            for (auto missile : game.missile_manager.get_attack_missiles())
            {
                if (missile->get_target() == city.get_position())
                {
                    missile_count++;
                }
            }

            if (missile_count == 0)
            {
                selected_info_window.print_spaces(5, COLOR_PAIR(4));
                selected_info_window.print_left(5, "No missiles targeting the city", COLOR_PAIR(4));
            }
            else if (missile_count < 3)
            {
                selected_info_window.print_spaces(5, COLOR_PAIR(3));
                selected_info_window.print_left(5, std::to_string(missile_count) + " approaching the city", COLOR_PAIR(3));
            }
            else
            {
                selected_info_window.print_spaces(5, COLOR_PAIR(2));
                selected_info_window.print_left(5, std::to_string(missile_count) + " Missiles Approaching !!!", COLOR_PAIR(2));
            }
        }
    }
    else
    {
        selected_info_window.print_left(0, "Nothing Selected Now", A_NORMAL);
    }

    // NOTE: draw tech info window
    if (game.tech_tree.researching != nullptr)
    {
        tech_info_window.print_left(0, "Researching:", A_NORMAL);
        tech_info_window.print_left(1, "Remaining Time:", A_NORMAL);

        tech_info_window.print_right(0, game.tech_tree.researching->name, A_NORMAL);
        tech_info_window.print_right(1, std::to_string(game.tech_tree.remaining_time), A_NORMAL);
    }
    else
    {
        tech_info_window.print_left(0, "Not Researching", A_NORMAL);
    }

    tech_info_window.print_left(2, "Available:", A_NORMAL);
    tech_info_window.print_left(3, "Researched:", A_NORMAL);

    tech_info_window.print_right(2, "    ", game.tech_tree.available.size() > 0 ? COLOR_PAIR(4) : COLOR_PAIR(3));
    tech_info_window.print_right(2, std::to_string(game.tech_tree.available.size()), game.tech_tree.available.size() > 0 ? COLOR_PAIR(4) : COLOR_PAIR(3));
    tech_info_window.print_right(3, std::to_string(game.tech_tree.researched.size()));

    // NOTE: draw super weapon info window
    super_weapon_info_window.print_left(0, "Standard Bomb", A_NORMAL);
    if (game.standard_bomb_counter > 0)
    {
        super_weapon_info_window.print_right(0, "             ", COLOR_PAIR(3));
        super_weapon_info_window.print_right(0, "Remains " + std::to_string(game.standard_bomb_counter), COLOR_PAIR(3));
    }
    else if (game.standard_bomb_counter == 0)
    {
        super_weapon_info_window.print_right(0, "             ", COLOR_PAIR(4));
        super_weapon_info_window.print_right(0, "Ready", COLOR_PAIR(4));
    }
    else
    {
        super_weapon_info_window.print_right(0, "             ", COLOR_PAIR(2));
        super_weapon_info_window.print_right(0, "Not Built", COLOR_PAIR(2));
    }

    if (game.en_dirty_bomb)
    {
        super_weapon_info_window.print_left(1, "Dirty Bomb", A_NORMAL);
        if (game.dirty_bomb_counter > 0)
        {
            super_weapon_info_window.print_right(1, "             ", COLOR_PAIR(3));
            super_weapon_info_window.print_right(1, "Remains " + std::to_string(game.dirty_bomb_counter), COLOR_PAIR(3));
        }
        else if (game.dirty_bomb_counter == 0)
        {
            super_weapon_info_window.print_right(1, "             ", COLOR_PAIR(4));
            super_weapon_info_window.print_right(1, "Ready", COLOR_PAIR(4));
        }
        else
        {
            super_weapon_info_window.print_right(1, "             ", COLOR_PAIR(2));
            super_weapon_info_window.print_right(1, "Not Built", COLOR_PAIR(2));
        }
    }
    if (game.en_hydrogen_bomb)
    {
        super_weapon_info_window.print_left(2, "Hydrogen Bomb", A_NORMAL);
        if (game.hydrogen_bomb_counter > 0)
        {
            super_weapon_info_window.print_right(2, "             ", COLOR_PAIR(3));
            super_weapon_info_window.print_right(2, "Remains" + std::to_string(game.hydrogen_bomb_counter), COLOR_PAIR(3));
        }
        else if (game.hydrogen_bomb_counter == 0)
        {
            super_weapon_info_window.print_right(2, "             ", COLOR_PAIR(4));
            super_weapon_info_window.print_right(2, "Ready", COLOR_PAIR(4));
        }
        else
        {
            super_weapon_info_window.print_right(2, "             ", COLOR_PAIR(2));
            super_weapon_info_window.print_right(2, "Not Built", COLOR_PAIR(2));
        }
    }
    if (game.en_iron_curtain)
    {
        super_weapon_info_window.print_left(3, "Iron Curtain", A_NORMAL);
        if (game.iron_curtain_counter > 40)
        {
            super_weapon_info_window.print_right(3, "             ", COLOR_PAIR(4));
            super_weapon_info_window.print_right(3, "Remains" + std::to_string(game.iron_curtain_counter), COLOR_PAIR(4));
        }
        else if (game.iron_curtain_counter > 0)
        {
            super_weapon_info_window.print_right(3, "             ", COLOR_PAIR(3));
            super_weapon_info_window.print_right(3, "Remains" + std::to_string(game.iron_curtain_counter), COLOR_PAIR(3));
        }
        else
        {
            super_weapon_info_window.print_right(3, "             ", COLOR_PAIR(2));
            super_weapon_info_window.print_right(3, "Not Activated", COLOR_PAIR(2));
        }
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
            operation_window.print_left(index - menu.get_offset(), menu.get_item(index), A_REVERSE);
        }
        else
        {
            operation_window.print_left(index - menu.get_offset(), menu.get_item(index));
        }
    }

    // NOTE: draw feedback window
    for (int index = 0; index < game.get_feedbacks().size(); index++)
    {
        if (index >= feedback_size.h)
        {
            break;
        }
        const AttrString &feedback = game.get_feedbacks().at(game.get_feedbacks().size() - index - 1);
        feedback_window.print_spaces(index, feedback.attr);
        feedback_window.print_left(index, feedback);
    }
}
