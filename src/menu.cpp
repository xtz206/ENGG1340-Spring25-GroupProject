#include <string>
#include <vector>
#include <algorithm>

#include "menu.h"

Menu::Menu(const std::string &t, const std::vector<std::string> &it)
    : title(t), items(it), cursor(0)
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

SaveMenu::SaveMenu(const std::string &t, SaveDumper &sd)
    : BasicMenu(t, {}), save_dumper(sd),
      all_items({"RETURN TO MENU", "SLOT 1 EMPTY", "SLOT 2 EMPTY", "SLOT 3 EMPTY",
                 "SLOT 1  FULL", "SLOT 2  FULL", "SLOT 3  FULL"})
{
    items.push_back(all_items.at(0));
}

void SaveMenu::update_items(void)
{
    items.erase(items.begin() + 1, items.end());
    items.push_back(all_items.at(save_dumper.is_slot_empty("1") ? 1 : 4));
    items.push_back(all_items.at(save_dumper.is_slot_empty("2") ? 2 : 5));
    items.push_back(all_items.at(save_dumper.is_slot_empty("3") ? 3 : 6));
}

LoadMenu::LoadMenu(const std::string &t, SaveLoader &sl)
    : BasicMenu(t, {}), save_loader(sl),
      all_items({"RETURN TO MENU", "SLOT 1 EMPTY", "SLOT 2 EMPTY", "SLOT 3 EMPTY",
                 "SLOT 1  FULL", "SLOT 2  FULL", "SLOT 3  FULL"})
{
    items.push_back(all_items.at(0));
}
void LoadMenu::update_items(void)
{
    items.erase(items.begin() + 1, items.end());
    items.push_back(all_items.at(save_loader.is_slot_empty("1") ? 1 : 4));
    items.push_back(all_items.at(save_loader.is_slot_empty("2") ? 2 : 5));
    items.push_back(all_items.at(save_loader.is_slot_empty("3") ? 3 : 6));
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

TutorialMenu::TutorialMenu(void)
    : BasicMenu("TUTORIAL", {"NEXT PAGE", "PREV PAGE", "RETURN TO MENU"}), page_index(0),
      pages({{"W/A/S/D          Move Cursor", "Q             Prev Operation", "E             Next Operation",
              "SPACE              Next Turn", "ENTER       Select Operation", "ESC                Quit Game",
              "P                 Pause Game", "R            Technology Menu", "F                   Fix City",
              "B               Build Cruise", "L              Launch Cruise"}})
{
}

void TutorialMenu::next_page(void)
{
    if (page_index < pages.size() - 1)
        page_index++;
}

void TutorialMenu::prev_page(void)
{
    if (page_index > 0)
        page_index--;
}


std::string TutorialMenu::get_page_info() const
{
    return std::to_string(page_index + 1) + "/" + std::to_string(pages.size());
}
