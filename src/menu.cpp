#include <string>
#include <vector>
#include <algorithm>

#include "menu.h"

/**
 * @brief Initializes menu system with title and options.
 * Sets initial activation state and cursor position. Base class for all menu types.
 * @param t Title text displayed at menu top
 * @param it Collection of selectable menu entries
 */
Menu::Menu(const std::string &t, const std::vector<std::string> &it)
    : activated(false), title(t), items(it), cursor(0) // Initialize state flags
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
 * @brief Constructs dynamic operation selection interface.
 * Initializes core game actions menu with scroll support. Dynamically populates
 * available commands based on research progression.
 * @param g Game state reference for availability checks
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
 * @param t Reference to technology progression system
 * @param msg Informational message displayed at list top
 */
TechMenu::TechMenu(TechTree &t, const std::string &msg)
    : ScrollMenu("Technology", t.get_tech_names(), 10), tech_tree(t)
{
    items.insert(items.begin(), msg);
}

/**
 * @brief Generates detailed technology specifications.
 * Compiles research status, costs, prerequisites, and descriptions for selected
 * technology node. Handles special case for header message selection.
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
