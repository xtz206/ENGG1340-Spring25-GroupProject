#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

#include "game.h"

class Menu
{
protected:
    std::string title;
    std::vector<std::string> items;
    int cursor;

public:
    Menu(const std::string &t, const std::vector<std::string> &b);
    int get_cursor(void) const { return cursor; };
    virtual void move_cursor(int dcursor);

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

class TitleMenu : public Menu

{
public:
    TitleMenu(const std::vector<std::string> &t, const std::string &d);
};

class TitleVideo
{
private:
    size_t index;
    std::vector<std::vector<std::string>> frames;

public:
    TitleVideo(const std::vector<std::vector<std::string>> &fs) : index(0), frames(fs) {};
    int get_index(void) const { return index; };
    const std::vector<std::string> &get_frame(void) const { return frames.at(index); };
    const std::vector<std::string> &get_frame(int i) const { return frames.at(i); };
    void next_frame(void) { index = (index + 1) % frames.size(); };
    void prev_frame(void) { index = (index - 1 + frames.size()) % frames.size(); };
    bool is_end(void) const { return index == frames.size() - 1; };
};

class SaveMenu : public BasicMenu
{
private:
    SaveDumper &save_dumper;
    std::vector<std::string> all_items;

public:
    SaveMenu(const std::string &t, SaveDumper &sd);
    void update_items(void);
};

class LoadMenu : public BasicMenu
{
private:
    SaveLoader save_loader;
    std::vector<std::string> all_items;

public:
    LoadMenu(const std::string &t, SaveLoader &sl);
    void update_items(void);
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

class TutorialMenu : public BasicMenu
{
    std::vector<std::vector<std::string>> pages;
    int page_index;

public:
    TutorialMenu(void);
    void prev_page(void);
    void next_page(void);
    const std::vector<std::string> &get_page() const { return pages.at(page_index); };
    std::string get_page_info() const;
};

#endif