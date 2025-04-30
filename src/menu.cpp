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

void ScrollMenu::move_cursor(int dcursor)
{
    if (cursor + dcursor < 0 || cursor + dcursor >= items.size())
    {
        return;
    }
    if (cursor - offset + dcursor < 0)
    {
        offset += dcursor;
    }
    else if (cursor - offset + dcursor >= limit)
    {
        offset += dcursor;
    }
    cursor += dcursor;
}

OperationMenu::OperationMenu(Game &g)
    : ScrollMenu("Operation", {}, 9), game(g),
      all_items({"RESEARCH", "FIX", "BUILD CRUISE", "LAUNCH CRUISE", "BUILD STANDARD BOMB", "LAUNCH STANDARD BOMB",
                 "BUILD DIRTY BOMB", "LAUNCH DIRTY BOMB", "BUILD HYDROGEN BOMB", "LAUNCH HYDROGEN BOMB", "ACTIVATE IRON CURTAIN"})
{
    for (int index = 0; index < 6; index++)
    {
        items.push_back(all_items.at(index));
    }
}

void OperationMenu::update_items(void)
{
    items.erase(items.begin() + 6, items.end());
    if (game.en_dirty_bomb)
    {
        items.push_back(all_items.at(6));
        items.push_back(all_items.at(7));
    }
    if (game.en_hydrogen_bomb)
    {
        items.push_back(all_items.at(8));
        items.push_back(all_items.at(9));
    }
    if (game.en_iron_curtain)
    {
        items.push_back(all_items.at(10));
    }
}

TechMenu::TechMenu(TechTree &t, const std::string &msg)
    : ScrollMenu("Technology", t.get_tech_names(), 10), tech_tree(t)
{
    items.insert(items.begin(), msg);
}

std::vector<std::string> TechMenu::get_item_description()
{
    if (cursor == 0)
    {
        return {};
    }

    TechNode *node = get_tech_node();
    std::vector<std::string> description;
    description.push_back("Name: " + node->name);
    description.push_back("Description:");
    for (auto line : node->description)
    {
        description.push_back(line);
    }
    description.push_back("Cost: " + std::to_string(node->cost));
    description.push_back("Time: " + std::to_string(node->time));
    description.push_back("Prerequisites:");
    for (auto prerequisite : node->prerequisites)
    {
        description.push_back(prerequisite->name);
    }
    if (node->prerequisites.empty())
    {
        description.push_back("None");
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

TutorialMenu::TutorialMenu(const std::vector<std::vector<std::string>> &content)
    : BasicMenu("TUTORIAL", {"Next Page", "Previous Page", "Return"}), pages(content)
{
}

void TutorialMenu::next_page(void)
{
    if (current_page < pages.size() - 1)
        current_page++;
}

void TutorialMenu::prev_page(void)
{
    if (current_page > 0)
        current_page--;
}

const std::vector<std::string> &TutorialMenu::get_content() const
{
    return pages[current_page];
}

std::string TutorialMenu::get_page_info() const
{
    return std::to_string(current_page + 1) + "/" + std::to_string(pages.size());
}
