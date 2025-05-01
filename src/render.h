#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <ncurses.h>
#include "utils.h"

class Window
{
private:
    WINDOW *window;
    Size size;
    Position pos;

public:
    Window(WINDOW *win, Size s, Position p);
    Window(Window &win, Size s, Position p);
    ~Window(void) { delwin(window); };
    void refresh(void) { wrefresh(window); };
    void erase(void) { werase(window); };
    void draw_margin(void) { box(window, 0, 0); };
    void draw_hline(Position p, int len) { mvwhline(window, p.y, p.x, ACS_HLINE, len); };
    void draw_vline(Position p, int len) { mvwvline(window, p.y, p.x, ACS_VLINE, len); };
    void draw_char(Position p, chtype ch) { mvwaddch(window, p.y, p.x, ch); };
    void print_move(Position p, const std::string &s, attr_t attr = A_NORMAL);
    void print_left(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_center(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_right(int line, const std::string &s, attr_t attr = A_NORMAL);
};

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

    Size size;
    Position pos;

    Window box_window;
    Window item_window;

public:
    BasicMenuRenderer(Menu &m, Size s);

    void init(void);
    void render(void);
    void draw(void);
};

class EndMenuRenderer : public Renderer
{
private:
    Game &game;
    BasicMenu &menu;

    Size desc_size;
    Size item_size;
    Position pos;

    Window box_window;
    Window desc_window;
    Window item_window;

public:
    EndMenuRenderer(Game &g, BasicMenu &m, Size ds, Size is);

    void init(void);
    void render(void);
    void draw(void);
};

class TutorialMenuRenderer : public Renderer
{
private:
    TutorialMenu &menu;

    Size page_size;
    Size item_size;
    Position pos;

    Window box_window;
    Window page_window;
    Window item_window;

public:
    TutorialMenuRenderer(TutorialMenu &m, Size ps, Size is);

    void init(void);
    void render(void);
    void draw(void);
};

class TechMenuRenderer : public Renderer
{
private:
    TechMenu &menu;

    Size item_size;
    Size desc_size;
    Position pos;

    Window box_window;
    Window item_window;
    Window desc_window;

public:
    TechMenuRenderer(TechMenu &m, Size is, Size ds);

    void init(void);
    void render(void);
    void draw(void);
};

class GameRenderer : public Renderer
{
private:
    Game &game;
    OperationMenu &menu;

    Size map_size;
    Size info_size;
    Size operation_size;
    Size feedback_size;
    std::vector<int> fields;
    Position pos;

    Window box_window;
    Window map_window;
    Window info_window;
    Window general_info_window;
    Window selected_info_window;
    Window tech_info_window;
    Window super_weapon_info_window;
    Window operation_window;
    Window feedback_window;

public:
    GameRenderer(Game &g, OperationMenu &m, Size s, const std::vector<int> &ls);

    void init(void);
    void render(void);
    void draw(void);
};

#endif
