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
    virtual void move_cursor(int dcursor);
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };

    const std::string &get_title(void) const { return title; };
    const std::vector<std::string> &get_items(void) const { return items; };
    const std::string &get_item(void) const { return items.at(cursor); };
    const std::string &get_item(int index) const { return items.at(index); };
};

class BasicMenu : public Menu
{

public:
    BasicMenu(const std::string &t, const std::vector<std::string> &it) : Menu(t, it) {};
};

class ScrollMenu : public Menu
{
private:
    int limit;
    int offset;

public:
    ScrollMenu(const std::string &t, const std::vector<std::string> &it, int lim) : Menu(t, it), limit(lim), offset(0) {};
    virtual void move_cursor(int dcursor) override;
    int get_offset(void) const { return offset; };
    int get_limit(void) const { return limit; };
};

class OperationMenu : public ScrollMenu
{
private:
    Game &game;
    std::vector<std::string> all_items;

public:
    OperationMenu(Game &g);
    void update_items(void);
};

class TechMenu : public ScrollMenu
{
private:
    TechTree &tech_tree;

public:
    TechMenu(TechTree &t, const std::string &msg);
    bool check_tech_node(void) const { return cursor > 0 && cursor <= tech_tree.nodes.size(); };
    TechNode *get_tech_node(void) const { return tech_tree.nodes.at(cursor - 1); };
    std::vector<std::string> get_item_description(void);
};
#endif