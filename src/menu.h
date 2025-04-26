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

    virtual const std::string &get_title(void) const { return title; };
    virtual const std::vector<std::string> &get_items(void) const { return items; };
    virtual const std::string &get_item(int index) const { return items.at(index); };
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
    OperationMenu(Game &g, int lim);
    virtual const std::vector<std::string> &get_items(void) const override { return items; };
    virtual const std::string &get_item(int index) const override { return items.at(index); };
    void update_items(void);
};

class TechMenu : public ScrollMenu
{
private:
    TechTree &tech_tree;

public:
    TechMenu(TechTree &t);
    TechNode *get_tech_node(void) const { return tech_tree.nodes.at(cursor - 1); };
    std::vector<std::string> get_item_description(void);
};
#endif