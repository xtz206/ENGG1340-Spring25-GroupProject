#include <string>
#include <vector>
#include <algorithm>

#include "menu.h"

/**
 * @brief Initializes menu system with title and items.
 * Sets initial activation state and cursor position. Base class for all menu types.
 * @param t Title text displayed at menu top
 * @param it Collection of selectable menu entries
 */
Menu::Menu(const std::string &t, const std::vector<std::string> &it)
    : title(t), items(it), cursor(0) // Initialize state flags
{
}

/**
 * @brief Adjusts navigation cursor position with boundary checks.
 * Ensures cursor remains within valid item range using modulo arithmetic.
 * @param dcursor Relative movement value (-1 for up, 1 for down)
 */
void Menu::move_cursor(int dcursor)
{
    // Validate movement within array bounds
    if (cursor + dcursor < 0 || cursor + dcursor >= items.size())
    {
        return; // Ignore invalid movements
    }
    cursor += dcursor;
}

/**
 * @brief Handles scroll-aware cursor navigation.
 * Manages visible window offset when cursor moves beyond display limits. Maintains
 * visual continuity for long item lists through viewport adjustments.
 * @param dcursor Relative movement value (-1/1 typical)
 */
void ScrollMenu::move_cursor(int dcursor)
{
    if (cursor + dcursor < 0 || cursor + dcursor >= items.size())
    {
        return; // Prevent out-of-bounds access
    }
    // Adjust scroll offset when approaching window edges
    if (cursor - offset + dcursor < 0)
    {
        offset += dcursor;
    }
    else if (cursor - offset + dcursor >= limit) // Bottom boundary check
    {
        offset += dcursor;
    }
    cursor += dcursor;
}

/**
 * @brief Constructs a TitleMenu object with a title and a description.
 *
 * @param t A vector of strings representing the title items.
 * @param d A string representing the description to be added to the menu.
 */
TitleMenu::TitleMenu(const std::vector<std::string> &t, const std::string &d)
    : Menu("", t)
{
    items.push_back("");
    items.push_back("");
    items.push_back("");
    items.push_back(d);
}

/**
 * @brief Constructs a SaveMenu object with a title and a reference to a SaveDumper object.
 *
 * @param t The title of the menu.
 * @param sd A reference to a SaveDumper object used for handling save operations.
 */
SaveMenu::SaveMenu(const std::string &t, SaveDumper &sd)
    : BasicMenu(t, {}), save_dumper(sd),
      all_items({"RETURN TO MENU", "SLOT 1 EMPTY", "SLOT 2 EMPTY", "SLOT 3 EMPTY",
                 "SLOT 1  FULL", "SLOT 2  FULL", "SLOT 3  FULL"})
{
    items.push_back(all_items.at(0));
}

/**
 * @brief Updates the items in the SaveMenu based on the save status of each slot.
 * Removes all items except the first one and adds the status of each save slot.
 */
void SaveMenu::update_items(void)
{
    items.erase(items.begin() + 1, items.end());
    items.push_back(all_items.at(save_dumper.is_slot_empty("1") ? 1 : 4));
    items.push_back(all_items.at(save_dumper.is_slot_empty("2") ? 2 : 5));
    items.push_back(all_items.at(save_dumper.is_slot_empty("3") ? 3 : 6));
}

/**
 * @brief Constructs a LoadMenu object with a title and a reference to a SaveLoader object.
 *
 * @param t The title of the menu.
 * @param sl A reference to a SaveLoader object used for loading game saves.
 */
LoadMenu::LoadMenu(const std::string &t, SaveLoader &sl)
    : BasicMenu(t, {}), save_loader(sl),
      all_items({"RETURN TO MENU", "SLOT 1 EMPTY", "SLOT 2 EMPTY", "SLOT 3 EMPTY",
                 "SLOT 1  FULL", "SLOT 2  FULL", "SLOT 3  FULL"})
{
    items.push_back(all_items.at(0));
}

/**
 * @brief Updates the items in the LoadMenu based on the save status of each slot.
 * Removes all items except the first one and adds the status of each load slot.
 */
void LoadMenu::update_items(void)
{
    items.erase(items.begin() + 1, items.end());
    items.push_back(all_items.at(save_loader.is_slot_empty("1") ? 1 : 4));
    items.push_back(all_items.at(save_loader.is_slot_empty("2") ? 2 : 5));
    items.push_back(all_items.at(save_loader.is_slot_empty("3") ? 3 : 6));
}

/**
 * @brief Constructs an OperationMenu object with a reference to the game instance.
 *
 * @param g Reference to the Game instance
 */
OperationMenu::OperationMenu(Game &g)
    : ScrollMenu("Operation", {}, 9), game(g), // Initialize scroll parameters
      all_items({"RESEARCH", "FIX", "BUILD CRUISE", "LAUNCH CRUISE", "BUILD STANDARD BOMB", "LAUNCH STANDARD BOMB",
                 "BUILD DIRTY BOMB", "LAUNCH DIRTY BOMB", "BUILD HYDROGEN BOMB", "LAUNCH HYDROGEN BOMB", "ACTIVATE IRON CURTAIN"})
{
    // Populate initial visible items (index 0-5)
    for (int index = 0; index < 6; index++)
    {
        items.push_back(all_items.at(index));
    }
}

/**
 * @brief Synchronizes menu options with current research status.
 * Dynamically appends advanced commands when corresponding technologies become
 * available. Maintains core commands in fixed positions.
 */
void OperationMenu::update_items(void)
{
    items.erase(items.begin() + 6, items.end());
    // Append dirty bomb operations when researched
    if (game.en_dirty_bomb)
    {
        items.push_back(all_items.at(6)); // Build dirty bomb
        items.push_back(all_items.at(7)); // Launch dirty bomb
    }
    // Append hydrogen bomb operations when researched
    if (game.en_hydrogen_bomb)
    {
        items.push_back(all_items.at(8)); // Build hydrogen bomb
        items.push_back(all_items.at(9)); // Launch hydrogen bomb
    }
    // Append iron curtain operation when researched
    if (game.en_iron_curtain)
    {
        items.push_back(all_items.at(10)); // Activate iron curtain
    }
}

/**
 * @brief Creates technology research interface.
 * Prepends system message to technology list. Inherits scroll behavior for
 * lengthy research trees.
 * @param t Reference to technology tree
 * @param msg Informational message displayed at list top
 */
TechMenu::TechMenu(TechTree &t, const std::string &msg)
    : ScrollMenu("Technology", t.get_tech_names(), 10), tech_tree(t)
{
    items.insert(items.begin(), msg);
}

/**
 * @brief Generates detailed technology specifications.
 * Display research status, costs, prerequisites, and descriptions for selected tech node.
 * Handles special case for header message selection.
 * @return std::vector<std::string> Formatted lines for display
 */
std::vector<std::string> TechMenu::get_item_description()
{
    if (cursor == 0) // Header message selected
    {
        return {}; // No description available
    }

    TechNode *node = get_tech_node();
    std::vector<std::string> description;
    description.push_back("Name: " + node->name); // Display technical name
    description.push_back("Description:");        // Section header
    for (auto line : node->description)           // Section header
    {
        description.push_back(line);
    }
    description.push_back("Cost: " + std::to_string(node->cost)); // Resource requirement
    description.push_back("Time: " + std::to_string(node->time)); // Research duration
    description.push_back("Prerequisites:");
    for (auto prerequisite : node->prerequisites)
    {
        description.push_back(prerequisite->name);
    }
    if (node->prerequisites.empty()) // Handle no-prerequisite case
    {
        description.push_back("None");
    }
    // Check if the technology is available
    if (tech_tree.researching == node)
    {
        description.push_back("Researching");
    }
    else if (tech_tree.is_researched(node)) // Check if the technology is already researched
    {
        description.push_back("Researched");
    }
    else if (tech_tree.is_available(node))
    {
        description.push_back("Available");
    }
    else
    {
        description.push_back("Not Available");
    }

    return description;
}

/**
 * @brief Constructs a TutorialMenu object.
 *
 * Initializes the tutorial menu with a title "TUTORIAL" and a set of menu options:
 * "NEXT PAGE", "PREV PAGE", and "RETURN TO MENU". It also sets the initial page index
 * to 0 and populates the tutorial pages with instructions for various game controls.
 *
 */
TutorialMenu::TutorialMenu(void)
    : BasicMenu("TUTORIAL", {"NEXT PAGE", "PREV PAGE", "RETURN TO MENU"}), page_index(0),
      pages({})
{
    std::vector<std::string> basic_keyboard_control_page = {
        "============= BASIC KEYBOARD CONTROLS ============",
        "W/A/S/D                                Move Cursor",
        "Q                                   Prev Operation",
        "E                                   Next Operation",
        "SPACE                                    Next Turn",
        "ENTER                             Select Operation",
        "ESC                                      Quit Game",
        "P                                       Pause Game"};
    std::vector<std::string> shortcut_keyboard_control_page = {
        "=========== SHORTCUT KEYBOARD CONTROLS ===========",
        "R                                    Research Menu",
        "F                                         Fix City",
        "B                                     Build Cruise",
        "L                                    Launch Cruise",
        "1-9                                   Select City"};
    std::vector<std::string> game_target_page = {
        "=================== GAME TARGET ==================",
        "manage your deposit and resources wisely          ",
        "think carefully before investing in technology    ",
        "",
        "build cruise missiles to intercept enemy's attack ",
        "protect your city and  the people living there    ",
        "",
        "research and build the super weapons              ",
        "use them wisely to defeat the enemy!              "};
    std::vector<std::string> tip_page = {
        "====================== TIPS ======================",
        "use the first 40 turns to build missiles          ",
        "the enemy will attack every 40 turns              ",
        "build cruises when enemy approaches is too late   ",
        "",
        "fix city is quite expensive, use it wisely        ",
        "",
        "the technology menu is scrollable, you can find   ",
        "more powerful techs when you scroll down menu     "};

    pages.push_back(basic_keyboard_control_page);
    pages.push_back(shortcut_keyboard_control_page);
    pages.push_back(game_target_page);
    pages.push_back(tip_page);
}

/**
 * @brief Advances to the next page in the tutorial menu if there are more pages available.
 */
void TutorialMenu::next_page(void)
{
    if (page_index < pages.size() - 1)
        page_index++;
}
/**
 * @brief Returns to the previous page in the tutorial menu if there are previous pages available.
 */
void TutorialMenu::prev_page(void)
{
    if (page_index > 0)
        page_index--;
}

/**
 * @brief Retrieves the current page information.
 * @return std::string Formatted string indicating current page and total pages
 */
std::string TutorialMenu::get_page_info() const
{
    return std::to_string(page_index + 1) + "/" + std::to_string(pages.size());
}
