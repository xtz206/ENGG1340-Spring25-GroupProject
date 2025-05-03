/**
 * @file render.h
 * @brief Declares rendering components for game UI using ncurses
 */

#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "utils.h"

/**
 * @class Window
 * @brief Encapsulates ncurses window management
 * 
 * @var WINDOW *window Pointer to the ncurses window
 * @var Size size Size of the window
 * @var Position pos Position of the window on the screen
 * 
 * Provides methods for drawing characters, lines, and text within the window.
 * Also includes methods for refreshing and erasing the window.
 */
class Window
{
private:
    WINDOW *window;
    Size size;
    Position pos;

public:

    Window(WINDOW *win, Size s, Position p);
    Window(Window &win, Size s, Position p);
    ~Window(void) { delwin(window); };

    /// @name refresh/erase
    /// @{
    void refresh(void) { wrefresh(window); };
    void erase(void) { werase(window); };
    /// @}
    
    /// @name margin/line drawing
    /// @{
    void draw_margin(void) { box(window, 0, 0); };
    void draw_hline(Position p, int len) { mvwhline(window, p.y, p.x, ACS_HLINE, len); };
    void draw_vline(Position p, int len) { mvwvline(window, p.y, p.x, ACS_VLINE, len); };
    void draw_char(Position p, chtype ch) { mvwaddch(window, p.y, p.x, ch); };
    /// @}
    
    /// @name char/text printing
    /// @{
    void print(Position p, chtype ch, attr_t attr = A_NORMAL);
    void print(Position p, const char *s, attr_t = A_NORMAL);
    void print(Position p, const std::string &s, attr_t attr = A_NORMAL) { print(p, s.c_str(), attr); };
    void print(Position p, const AttrString &s) { print(p, s.str, s.attr); };
    void print_spaces(int line, attr_t = A_NORMAL);
    void print_left(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_left(int line, const AttrString &s) { print_left(line, s.str, s.attr); };
    void print_center(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_center(int line, const AttrString &s) { print_center(line, s.str, s.attr); };
    void print_right(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_right(int line, const AttrString &s) { print_right(line, s.str, s.attr); };
    /// @}
};

/**
 * @class Renderer
 * @brief Abstract base class for all UI renderers
 *
 * Defines common interface for initializing, updating and drawing
 * ncurses-based UI components. Provides debug utilities.
 */
class Renderer
{
public:
    /**
     * @brief Main rendering entry point
     * @details Updates all visual components
     */
    virtual void render(void) = 0;
    /**
     * @brief Initialize renderer resources
     * @details Creates windows and sets up initial UI state
     */
    virtual void init(void) = 0;
    /**
     * @brief Commit changes to physical screen
     * @details Performs final refresh operations on all windows
     */
    virtual void draw(void) = 0;

    /**
     * @brief Output debug information
     * @param str Debug message to display
     * @param line Optional line number (0-based) in debug area
     */
    void debug(const std::string &str, int line = 0);
};

/**
 * @class BasicMenuRenderer
 * @brief Renders standard menu interface
 *
 * Handles display of basic menu boxes and selectable items
 */
class BasicMenuRenderer : public Renderer
{
protected:
    Menu &menu;

    Size size;
    Position pos;

    Window box_window;
    Window item_window;

public:
    BasicMenuRenderer(Menu &m, Size s);

    void init(void);
    void render(void);
    void draw(void);
};

class TitleMenuRenderer : public Renderer
{
private:
    TitleMenu &menu;

    Size size;
    Position pos;

    Window title_window;

public:
    TitleMenuRenderer(TitleMenu &m, Size s);

    void init(void);
    void render(void);
    void draw(void);
};

/**
 * @class SaveMenuRenderer
 * @brief Renders save/load game menu interface
 * 
 * Displays available save slots and their status
 * Handles user input for selecting save/load operations
 */
class SaveMenuRenderer : public BasicMenuRenderer
{

public:
    SaveMenuRenderer(Menu &m, Size s) : BasicMenuRenderer(m, s) {};

    void draw(void);
};

/**
 * @class EndMenuRenderer
 * @brief Renders end game menu interface
 * 
 * Displays game over options and player score/casualty statistics
 */
class EndMenuRenderer : public Renderer
{
private:
    Game &game;
    Menu &menu;

    Size desc_size;
    Size item_size;
    Position pos;

    Window box_window;
    Window desc_window;
    Window item_window;

public:
    EndMenuRenderer(Game &g, Menu &m, Size ds, Size is);

    void init(void);
    void render(void);
    void draw(void);
};

/**
 * @class TutorialMenuRenderer
 * @brief Specialized renderer for tutorial content
 *
 * Displays tutorial text with custom formatting and navigation
 */
class TutorialMenuRenderer : public Renderer
{
private:
    TutorialMenu &menu;

    Size page_size;
    Size item_size;
    Position pos;

    Window box_window;
    Window page_window;
    Window item_window;

public:
    TutorialMenuRenderer(TutorialMenu &m, Size ps, Size is);

    void init(void);
    void render(void);
    void draw(void);
};

/**
 * @class TechMenuRenderer
 * @brief Renders technology tree interface
 *
 * Displays tech nodes and descriptions with visual connections
 */
class TechMenuRenderer : public Renderer
{
private:
    TechMenu &menu;

    Size item_size;
    Size desc_size;
    Position pos;

    Window box_window;
    Window item_window;
    Window desc_window;

public:
    TechMenuRenderer(TechMenu &m, Size is, Size ds);

    void init(void);
    void render(void);
    void draw(void);
};

/**
 * @class GameRenderer
 * @brief Main game interface renderer
 *
 * Manages complex layout of game view including:
 * - Strategic map
 * - Status panels
 * - Operation controls
 * - Feedback system
 */
class GameRenderer : public Renderer
{
private:
    Game &game;          ///< Game state reference
    OperationMenu &menu; ///< Operation controls data

    Size map_size;
    Size info_size;
    Size operation_size;
    Size feedback_size;
    std::vector<int> fields;
    Position pos;

    Window box_window; ///< Master border window
    Window map_window; ///< Tactical map display
    Window info_window;
    Window general_info_window;      ///< Player stats/status
    Window selected_info_window;     ///< Selected entity details
    Window tech_info_window;         ///< Research progress
    Window super_weapon_info_window; ///< Special weapons status
    Window operation_window;         ///< Command interface
    Window feedback_window;          ///< System messages/notifications

public:
    GameRenderer(Game &g, OperationMenu &m, Size s, const std::vector<int> &ls);

    void init(void);
    void render(void);
    void draw(void);
};

#endif
