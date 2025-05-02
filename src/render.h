/**
 * @file render.h
 * @brief Declares rendering components for game UI using ncurses
 */
#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <ncurses.h>

// Forward declarations
class Menu;
class Game;

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
private:
    Menu &menu;             ///< Menu data source
    WINDOW *box_window;     ///< Outer border window
    WINDOW *item_window;    ///< Menu items display window

public:
    /**
     * @brief Construct a new BasicMenuRenderer
     * @param m Reference to menu data model
     */
    BasicMenuRenderer(Menu &m) : menu(m) {};

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
    TutorialMenu &menu;   ///< Tutorial content container
    WINDOW *box_window;   ///< Main content border
    WINDOW *item_window;   ///< Text display area

public:
    /**
     * @brief Construct a new TutorialMenuRenderer
     * @param m Reference to tutorial content
     */
    TutorialMenuRenderer(TutorialMenu &m) : menu(m) {};

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
    TechMenu &menu;         ///< Tech tree data source
    WINDOW *box_window;     ///< Main container window
    WINDOW *item_window;    ///< Tech node list window
    WINDOW *desc_window;    ///< Technology description window

public:
    /**
     * @brief Construct a new TechMenuRenderer
     * @param m Reference to tech tree data
     */
    TechMenuRenderer(TechMenu &m) : menu(m) {};

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
    Game &game; ///< Game state reference
    OperationMenu &menu; ///< Operation controls data
    WINDOW *box_window; ///< Master border window
    WINDOW *map_window; ///< Tactical map display
    WINDOW *general_info_window; ///< Player stats/status
    WINDOW *selected_info_window; ///< Selected entity details
    WINDOW *tech_info_window; ///< Research progress
    WINDOW *super_weapon_info; ///< Special weapons status
    WINDOW *operation_window; ///< Command interface
    WINDOW *feedback_window; ///< System messages/notifications


public:
    /**
     * @brief Construct a new GameRenderer
     * @param g Main game state reference
     * @param m Operation menu data source
     */
    GameRenderer(Game &g, OperationMenu &m) : game(g), menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

#endif
