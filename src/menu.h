#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

class Menu
{
private:
    bool activated;
    std::string title;
    std::vector<std::string> buttons;
    int cursor;
    int limit;

public:
    Menu(const std::string &t, const std::vector<std::string> &b, int limit);
    int get_cursor(void) const { return cursor; };
    void move_cursor(int dcursor);
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };

    const std::string &get_title(void) const { return title; };
    const std::vector<std::string> &get_buttons(void) const { return buttons; };
};

#endif