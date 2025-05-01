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
    void print(Position p, chtype ch, attr_t attr = A_NORMAL);
    void print(Position p, const char *s, attr_t = A_NORMAL);
    void print(Position p, const std::string &s, attr_t attr = A_NORMAL) { print(p, s.c_str(), attr); };
    void print(Position p, const AttrString &s) { print(p, s.str, s.attr); };
    void print_left(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_left(int line, const AttrString &s) { print_left(line, s.str, s.attr); };
    void print_center(int line, const std::string &s, attr_t attr = A_NORMAL);
    void print_center(int line, const AttrString &s) { print_center(line, s.str, s.attr); };
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
protected:
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

class TitleMenuRenderer : public Renderer
{
private:
    TitleMenu &menu;

    Size size;
    Position pos;

    Window title_window;

public:
    TitleMenuRenderer(TitleMenu &m, Size s);

    void init(void);
    void render(void);
    void draw(void);
};

class SaveMenuRenderer : public BasicMenuRenderer
{

public:
    SaveMenuRenderer(Menu &m, Size s) : BasicMenuRenderer(m, s) {};

    void draw(void);
};

class EndMenuRenderer : public Renderer
{
private:
    Game &game;
    Menu &menu;

    Size desc_size;
    Size item_size;
    Position pos;

    Window box_window;
    Window desc_window;
    Window item_window;

public:
    EndMenuRenderer(Game &g, Menu &m, Size ds, Size is);

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
