#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <ncurses.h>

class Menu;
class Game;

class Renderer
{
public:
    virtual void render(void) = 0;
    virtual void init(void) = 0;
    virtual void draw(void) = 0;
    void debug(const std::string &str, int line = 0);
};

class BasicMenuRenderer : public Renderer
{
private:
    Menu &menu;
    WINDOW *box_window;
    WINDOW *item_window;

public:
    BasicMenuRenderer(Menu &m) : menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

class TutorialMenuRenderer : public Renderer
{
private:
    TutorialMenu &menu;
    WINDOW *box_window;
    WINDOW *item_window;

public:
    TutorialMenuRenderer(TutorialMenu &m) : menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

class TechMenuRenderer : public Renderer
{
private:
    TechMenu &menu;
    WINDOW *box_window;
    WINDOW *item_window;
    WINDOW *desc_window;

public:
    TechMenuRenderer(TechMenu &m) : menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

class GameRenderer : public Renderer
{
private:
    Game &game;
    OperationMenu &menu;
    WINDOW *box_window;
    WINDOW *map_window;
    WINDOW *general_info_window;
    WINDOW *selected_info_window;
    WINDOW *tech_info_window;
    WINDOW *super_weapon_info;
    WINDOW *operation_window;
    WINDOW *feedback_window;

public:
    GameRenderer(Game &g, OperationMenu &m) : game(g), menu(m) {};

    void init(void);
    void render(void);
    void draw(void);
};

#endif
