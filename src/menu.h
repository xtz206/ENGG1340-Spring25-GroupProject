/**
 * @file menu.h
 * @brief Declares menu system components for game interfaces
 */

#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

#include "game.h"

/**
 * @class Menu
 * @brief Base class for all menu interfaces
 *
 * Provides core functionality for menu navigation and item selection.
 * Supports basic cursor movement and activation state management.
 */
class Menu
{
protected:
    std::string title;              ///< Header text display
    std::vector<std::string> items; ///< Selectable menu entries
    int cursor;                     ///< Current selection index

public:
    /**
     * @brief Construct a new Menu object
     * @param t Menu title text
     * @param b Collection of menu items
     */
    Menu(const std::string &t, const std::vector<std::string> &b);
    /// @name Cursor Operations
    /// @{
    int get_cursor(void) const { return cursor; }; ///< Get current selection index
    virtual void move_cursor(int dcursor);         ///< Move cursor by relative offset
    /// @}

    /// @name Content Access
    /// @{
    const std::string &get_title(void) const { return title; };               ///< Get menu header
    const std::vector<std::string> &get_items(void) const { return items; };  ///< Get all entries
    const std::string &get_item(void) const { return items.at(cursor); };     ///< Get current item
    const std::string &get_item(int index) const { return items.at(index); }; ///< Get by index
    /// @}
};

/**
 * @class BasicMenu
 * @brief Simple non-scrollable menu implementation
 *
 * Inherits core functionality from Menu without additional scrolling capabilities.
 * Suitable for small fixed-length selections.
 */
class BasicMenu : public Menu
{
public:
    /**
     * @brief Construct a new BasicMenu object
     * @param t Menu title text
     * @param it Menu items collection
     */
    BasicMenu(const std::string &t, const std::vector<std::string> &it) : Menu(t, it) {};
};

/**
 * @class ScrollMenu
 * @brief Scrollable menu implementation with viewport control
 *
 * Adds pagination support for large item collections through:
 * - Visible item limit
 * - Scroll offset tracking
 */
class ScrollMenu : public Menu
{
private:
    int limit;  ///< Maximum visible items at once
    int offset; ///< Starting index of visible items

public:
    /**
     * @brief Construct a new ScrollMenu object
     * @param t Menu title text
     * @param it Complete item collection
     * @param lim Maximum visible items
     */
    ScrollMenu(const std::string &t, const std::vector<std::string> &it, int lim) : Menu(t, it), limit(lim), offset(0) {};
    virtual void move_cursor(int dcursor) override;
    /// @name Viewport Control
    /// @{
    int get_offset(void) const { return offset; }; ///< Get current scroll position
    int get_limit(void) const { return limit; };   ///< Get visible item capacity
    /// @}
};

/**
 * @class TitleMenu
 * @brief Game title screen menu
 *
 * Displays game title and description.
 * Inherits from Menu for basic functionality.
 */
class TitleMenu : public Menu

{
public:
    TitleMenu(const std::vector<std::string> &t, const std::string &d);
};

/**
 * @class SaveMenu
 * @brief Game save file management menu
 *
 * Provides a list of available game saves for saving.
 * Automatically updates available saves through update_items().
 */
class SaveMenu : public BasicMenu
{
private:
    SaveDumper &save_dumper;
    std::vector<std::string> all_items;

public:
    SaveMenu(const std::string &t, SaveDumper &sd);
    void update_items(void);
};


/**
 * @class LoadMenu
 * @brief Game save file selection menu
 *
 * Provides a list of available game saves for loading.
 * Automatically updates available saves through update_items().
 */
class LoadMenu : public BasicMenu
{
private:
    SaveLoader save_loader;
    std::vector<std::string> all_items;

public:
    LoadMenu(const std::string &t, SaveLoader &sl);
    void update_items(void);
};

/**
 * @class OperationMenu
 * @brief Dynamic game command menu
 *
 * Maintains context-sensitive operations based on game state.
 * Automatically updates available commands through update_items().
 */
class OperationMenu : public ScrollMenu
{
private:
    Game &game;                         ///< Main game state reference
    std::vector<std::string> all_items; ///< Complete command list

public:
    /**
     * @brief Construct a new OperationMenu tied to game state
     * @param g Game context reference
     */
    OperationMenu(Game &g);
    /**
     * @brief Refresh menu items based on current game state
     *
     * Synchronizes available commands with game context and player capabilities.
     */
    void update_items(void);
};

/**
 * @class TechMenu
 * @brief Technology tree navigation menu
 *
 * Specialized scroll menu for displaying and interacting with research nodes.
 * Provides direct access to TechTree data structure.
 */
class TechMenu : public ScrollMenu
{
private:
    TechTree &tech_tree; ///< Research system data source

public:
    /**
     * @brief Construct a new TechMenu
     * @param t TechTree reference
     * @param msg Menu header message
     */
    TechMenu(TechTree &t, const std::string &msg);
    /// @name Tech Node Interaction
    /// @{
    bool check_tech_node(void) const { return cursor > 0 && cursor <= tech_tree.nodes.size(); }; ///< Validate cursor position against tech nodes
    TechNode *get_tech_node(void) const { return tech_tree.nodes.at(cursor - 1); };              ///< Get currently selected research node
    std::vector<std::string> get_item_description(void);                                         ///< Get detailed node info
    /// @}
};

/**
 * @class TutorialMenu
 * @brief Multi-page tutorial content viewer
 *
 * Organizes tutorial materials into navigable pages with
 * forward/backward browsing capabilities.
 */
class TutorialMenu : public BasicMenu
{
    std::vector<std::vector<std::string>> pages; ///< Paginated content storage
    int page_index;                              ///< Active page index

public:
    /**
     * @brief Construct a new TutorialMenu
     */
    TutorialMenu(void);
    /// @name Page Navigation
    /// @{
    void prev_page(void); ///< Move to previous content page
    void next_page(void); ///< Advance to next content page
    /// @}

    /// @name Content Access
    /// @{
    const std::vector<std::string> &get_page() const { return pages.at(page_index); }; ///< Get current page lines
    std::string get_page_info() const;                                                 ///< Get "Page X/Y" formatted string
    /// @}
};

#endif