#ifndef MENU_H
#define MENU_H

class Menu
{
private:
    bool activated;

public:
    Menu(void) : activated(false) {};
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };
};

#endif