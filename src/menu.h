#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

#include "game.h"

class Menu
{
protected:
    bool activated;
    std::string title;
    std::vector<std::string> items;
    int cursor;

public:
    Menu(const std::string &t, const std::vector<std::string> &b);
    int get_cursor(void) const { return cursor; };
    void move_cursor(int dcursor);
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };

    const std::string &get_title(void) const { return title; };
    const std::vector<std::string> &get_items(void) const { return items; };
};

class BasicMenu : public Menu
{

public:
    BasicMenu(const std::string &t, const std::vector<std::string> &it) : Menu(t, it) {};
};

class TechMenu : public Menu
{
private:
    TechTree &tech_tree;

public:
    TechMenu(TechTree &t);
    TechNode *get_tech_node(void) const { return tech_tree.nodes.at(cursor); };
    std::vector<std::string> get_item_description(void);
};
#endif