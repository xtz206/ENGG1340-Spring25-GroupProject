#include <string>
#include <vector>
#include <algorithm>

#include "menu.h"

Menu::Menu(const std::string &t, const std::vector<std::string> &it)
    : activated(false), title(t), items(it), cursor(0)
{
}

void Menu::move_cursor(int dcursor)
{
    if (cursor + dcursor < 0 || cursor + dcursor >= items.size())
    {
        return;
    }
    cursor += dcursor;
}

TechMenu::TechMenu(TechTree &t)
    : Menu("Technology", t.get_tech_names()), tech_tree(t)
{
}

std::vector<std::string> TechMenu::get_item_description()
{
    TechNode *node = get_tech_node();
    std::vector<std::string> description;
    description.push_back("Name: " + node->name);
    description.push_back("Description:");
    description.push_back(node->description);
    description.push_back("Cost: " + std::to_string(node->cost));
    description.push_back("Time: " + std::to_string(node->time));
    description.push_back("Prerequisites:");
    for (auto prerequisite : node->prerequisites)
    {
        description.push_back(prerequisite->name);
    }
    // Check if the technology is available
    if (tech_tree.researching == node)
    {
        description.push_back("Researching");
    }
    else if (tech_tree.is_researched(node))
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
