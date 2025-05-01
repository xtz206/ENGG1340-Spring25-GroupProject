#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ncurses.h>

#include "game.h"
#include "menu.h"
#include "render.h"

#define ALL_SIZE Size(LINES, COLS)

Window::Window(WINDOW *win, Size s, Position p) : size(s), pos(p)
{
    window = subwin(win, s.h, s.w, p.y, p.x);
}

Window::Window(Window &win, Size s, Position p) : size(s), pos(p)
{
    window = subwin(win.window, s.h, s.w, p.y, p.x);
}

void Window::print(Position p, chtype ch, attr_t attr)
{
    if (p.y >= size.h || p.x >= size.w)
    {
        return;
    }
    wattron(window, attr);
    mvwaddch(window, p.y, p.x, ch);
    wattroff(window, attr);
}

void Window::print(Position p, const char *s, attr_t attr)
{
    if (p.y >= size.h || p.x >= size.w)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, p.y, p.x, "%s", s);
    wattroff(window, attr);
}

void Window::print_spaces(int line, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, line, 0, "%s", std::string(size.w, ' ').c_str());
    wattroff(window, attr);
}

void Window::print_left(int line, const std::string &s, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, line, 0, "%s", s.c_str());
    if (s.length() > size.w)
    {
        mvwprintw(window, line, 0, "%s", s.substr(0, size.w - 1).c_str());
    }
    else
    {
        mvwprintw(window, line, 0, "%s", s.c_str());
    }
    wattroff(window, attr);
}

void Window::print_center(int line, const std::string &s, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }
    wattron(window, attr);
    mvwprintw(window, line, (size.w - s.length()) / 2, "%s", s.c_str());
    if (s.length() > size.w)
    {
        mvwprintw(window, line, 0, "%s", s.substr(0, size.w - 1).c_str());
    }
    else
    {
        mvwprintw(window, line, (size.w - s.length()) / 2, "%s", s.c_str());
    }
    wattroff(window, attr);
}

void Window::print_right(int line, const std::string &s, attr_t attr)
{
    if (line >= size.h)
    {
        return;
    }

    wattron(window, attr);
    if (s.length() > size.w)
    {
        mvwprintw(window, line, 0, "%s", s.substr(0, size.w - 1).c_str());
    }
    else
    {
        mvwprintw(window, line, size.w - s.length(), "%s", s.c_str());
    }
    mvwprintw(window, line, size.w - s.length(), "%s", s.c_str());
    wattroff(window, attr);
}

void Renderer::debug(const std::string &str, int line)
{
    mvwprintw(stdscr, line, 1, "%s", str.c_str());
}

BasicMenuRenderer::BasicMenuRenderer(Menu &m, Size s)
    : menu(m), size(s), pos((ALL_SIZE - s - Size(2, 2)) / 2),
      box_window(Window(stdscr, s + Size(2, 2), pos)),
      item_window(Window(box_window, s, pos + Size(1, 1)))
{
}

void BasicMenuRenderer::init(void)
{
    erase();
    box_window.draw_margin();
    box_window.print_center(0, menu.get_title());
}

void BasicMenuRenderer::render(void)
{
    item_window.refresh();
}

void BasicMenuRenderer::draw(void)
{
    item_window.erase();
    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }
}

TitleMenuRenderer::TitleMenuRenderer(TitleMenu &m, Size s)
    : menu(m), size(s), pos((ALL_SIZE - s) / 2),
      title_window(Window(stdscr, s, pos + Size(1, 1)))
{
}

void TitleMenuRenderer::init(void)
{
    erase();

    for (size_t index = 0; index < menu.get_items().size() - 1; index++)
    {
        title_window.print_left(index, menu.get_item(index));
    }

    title_window.print_center(size.h - 1, menu.get_item(menu.get_items().size() - 1), A_REVERSE);
}

void TitleMenuRenderer::render(void)
{
    title_window.refresh();
}

void TitleMenuRenderer::draw(void)
{
}

void SaveMenuRenderer::draw(void)
{
    item_window.erase();
    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }

    item_window.print_center(size.h - 2, "SAVE ON FULL WILL OVERWRITE", COLOR_PAIR(3));
}

EndMenuRenderer::EndMenuRenderer(Game &g, Menu &m, Size ds, Size is)
    : menu(m), game(g), desc_size(ds), item_size(is), pos((ALL_SIZE - Size(is.h + ds.h + 2, is.w + 2)) / 2),
      box_window(Window(stdscr, Size(is.h + ds.h + 3, is.w + 2), pos)),
      desc_window(Window(box_window, desc_size, pos + Size(1, 1))),
      item_window(Window(box_window, item_size, pos + Size(ds.h + 2, 1)))
{
}

void EndMenuRenderer::init(void)
{
    erase();

    box_window.draw_margin();
    box_window.print_center(0, menu.get_title());
    box_window.draw_hline(Size(desc_size.h + 1, 1), item_size.w);
    box_window.draw_char(Size(desc_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Size(desc_size.h + 1, item_size.w + 1), ACS_RTEE);

    std::ostringstream oss;

    if (game.get_enemy_hp() > 0)
    {
        desc_window.print_spaces(0, COLOR_PAIR(2));
        desc_window.print_center(0, "YOU LOSE", COLOR_PAIR(2));
    }
    else
    {
        desc_window.print_spaces(0, COLOR_PAIR(4));
        desc_window.print_center(0, "YOU  WIN", COLOR_PAIR(4));
    }

    desc_window.print_left(1, "Score:", A_NORMAL);
    if (game.get_score() > 1000)
    {
        oss << std::fixed << std::setprecision(2) << static_cast<double>(game.get_score()) / 1000 << "K";
        desc_window.print_right(1, oss.str(), COLOR_PAIR(4));
    }
    else if (game.get_score() > 100)
    {
        oss << game.get_score();
        desc_window.print_right(1, oss.str(), COLOR_PAIR(3));
    }
    else
    {
        oss << game.get_score();
        desc_window.print_right(1, oss.str(), COLOR_PAIR(2));
    }
    oss.str("");

    desc_window.print_left(2, "Casualty:", A_NORMAL);
    if (game.get_casualty() > 1000)
    {
        oss << std::fixed << std::setprecision(2) << static_cast<double>(game.get_casualty()) / 1000 << "M";
        desc_window.print_right(2, oss.str(), COLOR_PAIR(2));
    }
    else if (game.get_casualty() > 100)
    {
        oss << game.get_casualty() << "K";
        desc_window.print_right(2, oss.str(), COLOR_PAIR(3));
    }
    else
    {
        oss << game.get_casualty() << "K";
        desc_window.print_right(2, oss.str(), COLOR_PAIR(4));
    }
    oss.str("");

    desc_window.print_left(3, "Turn:", A_NORMAL);
    desc_window.print_right(3, std::to_string(game.get_turn()), A_NORMAL);
}

void EndMenuRenderer::render(void)
{
    item_window.refresh();
}

void EndMenuRenderer::draw(void)
{
    item_window.erase();

    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }
}

TutorialMenuRenderer::TutorialMenuRenderer(TutorialMenu &m, Size ps, Size is)
    : menu(m), page_size(ps), item_size(is), pos((ALL_SIZE - Size(ps.h + is.h + 3, ps.w + 2)) / 2),
      box_window(stdscr, Size(ps.h + is.h + 3, ps.w + 2), pos),
      page_window(box_window, ps, pos + Size(1, 1)),
      item_window(box_window, is, pos + Size(ps.y + 2, 1))
{
}

void TutorialMenuRenderer::init(void)
{
    erase();
    box_window.draw_margin();
    box_window.draw_hline(Size(page_size.h + 1, 1), page_size.w);
    box_window.draw_char(Size(page_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Size(page_size.h + 1, page_size.w + 1), ACS_RTEE);
    box_window.print_center(0, menu.get_title());
}

void TutorialMenuRenderer::render(void)
{
    page_window.refresh();
    item_window.refresh();
}

void TutorialMenuRenderer::draw(void)
{
    page_window.erase();
    item_window.erase();

    const std::vector<std::string> &page = menu.get_page();
    for (size_t index = 0; index < page.size(); index++)
    {
        page_window.print_center(index, page.at(index));
    }

    for (size_t index = 0; index < menu.get_items().size(); index++)
    {
        if (index == menu.get_cursor())
        {
            item_window.print_center(index, menu.get_item(index), A_REVERSE);
        }
        else
        {
            item_window.print_center(index, menu.get_item(index));
        }
    }
    item_window.print_center(item_size.h - 1, menu.get_page_info());
}

TechMenuRenderer::TechMenuRenderer(TechMenu &m, Size is, Size ds)
    : menu(m), item_size(is), desc_size(ds), pos((ALL_SIZE - Size(is.h + ds.h + 3, is.w + 2)) / 2),
      box_window(stdscr, Size(is.h + ds.h + 3, is.w + 2), pos),
      item_window(box_window, is, pos + Size(1, 1)),
      desc_window(box_window, ds, pos + Size(is.y + 2, 1))
{
}

void TechMenuRenderer::init(void)
{
    erase();
    box_window.draw_margin();
    box_window.print_center(0, menu.get_title());
    box_window.draw_hline(Size(item_size.h + 1, 1), item_size.w);
    box_window.draw_char(Size(item_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Size(item_size.h + 1, item_size.w + 1), ACS_RTEE);
}

void TechMenuRenderer::render(void)
{
    item_window.refresh();
    desc_window.refresh();
}

void TechMenuRenderer::draw(void)
{
    item_window.erase();
    desc_window.erase();

    for (int index = 0; index < menu.get_limit(); index++)
    {
        if (index + menu.get_offset() == menu.get_cursor())
        {
            item_window.print_left(index, menu.get_item(index + menu.get_offset()), A_REVERSE);
        }
        else
        {
            item_window.print_left(index, menu.get_item(index + menu.get_offset()));
        }
    }

    std::vector<std::string> description = menu.get_item_description();
    for (size_t index = 0; index < description.size(); index++)
    {
        desc_window.print_left(index, description.at(index));
    }
}

GameRenderer::GameRenderer(Game &g, OperationMenu &m, Size s, const std::vector<int> &fs)
    : game(g), menu(m), fields(fs),
      map_size(game.get_size()), info_size(map_size.h + s.h + 1, s.w),
      operation_size(s.h, map_size.w / 3),
      feedback_size(s.h, map_size.w - map_size.w / 3 - 1),
      pos((ALL_SIZE - map_size - s - Size(3, 3)) / 2),
      box_window(stdscr, map_size + s + Size(3, 3), pos),
      map_window(box_window, map_size, pos + Size(1, 1)),
      info_window(box_window, info_size, pos + Size(1, map_size.x + 2)),
      general_info_window(box_window, Size(fs.at(0), info_size.w), pos + Size(1, map_size.x + 2)),
      selected_info_window(box_window, Size(fs.at(1), info_size.w), pos + Size(fs.at(0) + 2, map_size.x + 2)),
      tech_info_window(box_window, Size(fs.at(2), info_size.w), pos + Size(fs.at(0) + fs.at(1) + 3, map_size.x + 2)),
      super_weapon_info_window(box_window, Size(fs.at(3), info_size.w), pos + Size(fs.at(0) + fs.at(1) + fs.at(2) + 4, map_size.x + 2)),
      operation_window(box_window, operation_size, pos + Size(map_size.y + 2, 1)),
      feedback_window(box_window, feedback_size, pos + Size(map_size.y + 2, operation_size.x + 2))
{
}

void GameRenderer::init(void)
{
    erase();

    box_window.draw_margin();
    box_window.draw_hline(Position(map_size.h + 1, 1), map_size.w);
    box_window.draw_vline(Position(1, map_size.w + 1), info_size.h);
    box_window.draw_vline(Position(map_size.h + 2, operation_size.w + 1), operation_size.h);
    box_window.draw_hline(Position(fields.at(0) + 1, map_size.w + 2), info_size.w);
    box_window.draw_hline(Position(fields.at(0) + fields.at(1) + 2, map_size.w + 2), info_size.w);
    box_window.draw_hline(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 2), info_size.w);

    box_window.draw_char(Position(map_size.h + 1, 0), ACS_LTEE);
    box_window.draw_char(Position(map_size.h + 1, map_size.w + 1), ACS_RTEE);
    box_window.draw_char(Position(0, map_size.w + 1), ACS_TTEE);
    box_window.draw_char(Position(info_size.h + 1, map_size.w + 1), ACS_BTEE);
    box_window.draw_char(Position(map_size.h + 1, operation_size.w + 1), ACS_TTEE);
    box_window.draw_char(Position(map_size.h + operation_size.h + 2, operation_size.w + 1), ACS_BTEE);

    box_window.draw_char(Position(fields.at(0) + 1, map_size.w + 1), ACS_LTEE);
    box_window.draw_char(Position(fields.at(0) + 1, map_size.w + info_size.w + 2), ACS_RTEE);
    box_window.draw_char(Position(fields.at(0) + fields.at(1) + 2, map_size.w + 1), ACS_LTEE);
    box_window.draw_char(Position(fields.at(0) + fields.at(1) + 2, map_size.w + info_size.w + 2), ACS_RTEE);
    if (fields.at(0) + fields.at(1) + fields.at(2) + 3 == map_size.h + 1)
    {
        box_window.draw_char(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 1), ACS_PLUS);
    }
    else
    {
        box_window.draw_char(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 1), ACS_LTEE);
    }
    box_window.draw_char(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + info_size.w + 2), ACS_RTEE);

    box_window.print(Position(0, 2), "Map");
    box_window.print(Position(0, map_size.w + 3), "General");
    box_window.print(Position(fields.at(0) + 1, map_size.w + 3), "City & Missile");
    box_window.print(Position(fields.at(0) + fields.at(1) + 2, map_size.w + 3), "Technology & Research");
    box_window.print(Position(fields.at(0) + fields.at(1) + fields.at(2) + 3, map_size.w + 3), "Super Weapon");
    box_window.print(Position(map_size.h + 1, 2), "Operation");
    box_window.print(Position(map_size.h + 1, operation_size.w + 3), "Feedback");
}

void GameRenderer::render(void)
{
    map_window.refresh();
    general_info_window.refresh();
    selected_info_window.refresh();
    tech_info_window.refresh();
    super_weapon_info_window.refresh();
    operation_window.refresh();
    feedback_window.refresh();
    // refresh();
}

void GameRenderer::draw(void)
{
    map_window.erase();
    general_info_window.erase();
    selected_info_window.erase();
    tech_info_window.erase();
    super_weapon_info_window.erase();
    operation_window.erase();
    feedback_window.erase();

    // NOTE: draw map window
    for (int line = 0; line < game.get_size().h; line++)
    {
        for (int col = 0; col < game.get_size().w; col++)
        {
            char ch = game.get_background().at(line).at(col);
            switch (ch)
            {
            case ' ':
                map_window.print(Position(line, col), " ", COLOR_PAIR(0));
                break;
            case '@':
                map_window.print(Position(line, col), "@", COLOR_PAIR(3));
                break;
            case '#':
                map_window.print(Position(line, col), " ", COLOR_PAIR(1));
                break;
            }
        }
    }

    for (auto missile : game.get_missiles())
    {
        if (!game.is_in_map(missile->get_position()))
        {
            continue;
        }
        if (missile->get_is_exploded())
        {
            continue;
        }
        std::string direction;
        switch (missile->get_direction())
        {
        case MissileDirection::A:
            direction = "O";
            break;
        case MissileDirection::N:
            direction = "↑ ";
            break;
        case MissileDirection::NE:
            direction = "↗ ";
            break;
        case MissileDirection::E:
            direction = "→ ";
            break;
        case MissileDirection::SE:
            direction = "↘ ";
            break;
        case MissileDirection::S:
            direction = "↓ ";
            break;
        case MissileDirection::SW:
            direction = "↙ ";
            break;
        case MissileDirection::W:
            direction = "← ";
            break;
        case MissileDirection::NW:
            direction = "↖ ";
            break;
        default:
            break;
        }

        map_window.print(missile->get_position(), direction, COLOR_PAIR(missile->get_type() == MissileType::ATTACK ? 2 : 3));
    }
    int color_pair = (game.is_on_land(game.get_cursor()) ? 0 : (game.is_on_sea(game.get_cursor()) ? 1 : 3));
    map_window.print(game.get_cursor(), "*", COLOR_PAIR(color_pair));

    // NOTE: draw general windows
    general_info_window.print_left(0, "Turn:", A_NORMAL);
    general_info_window.print_right(0, std::to_string(game.get_turn()), A_NORMAL);
    general_info_window.print_left(1, "Deposit:", A_NORMAL);
    general_info_window.print_right(1, std::to_string(game.get_deposit()), A_NORMAL);
    general_info_window.print_left(2, "Productivity:", A_NORMAL);
    general_info_window.print_right(2, std::to_string(game.get_productivity()), A_NORMAL);
    general_info_window.print_left(3, "Enemy HP:", A_NORMAL);
    general_info_window.print_right(3, std::to_string(game.get_enemy_hp()), A_NORMAL);
    if (game.en_self_defense_sys)
    {
        general_info_window.print_left(4, "Self Defense System:", A_NORMAL);
        general_info_window.print_right(4, "ON", A_NORMAL);
    }
    if (game.en_enhanced_radar_I)
    {
        int missile_count = game.missile_manager.get_attack_missiles().size();
        if (missile_count == 0)
        {
            general_info_window.print_spaces(5, COLOR_PAIR(4));
            general_info_window.print_left(5, "No Missiles Approaching", COLOR_PAIR(4));
        }
        else if (missile_count < 5)
        {
            general_info_window.print_spaces(5, COLOR_PAIR(3));
            general_info_window.print_left(5, std::to_string(missile_count) + " Missile Approaching", COLOR_PAIR(3));
        }
        else
        {
            general_info_window.print_spaces(5, COLOR_PAIR(2));
            general_info_window.print_left(5, std::to_string(missile_count) + " Missiles Approaching !!!", COLOR_PAIR(2));
        }
    }

    // NOTE: draw selected info window
    if (game.is_selected_missile())
    {
        AttackMissile &missile = dynamic_cast<AttackMissile &>(game.select_missile());
        selected_info_window.print_left(0, "Target:", A_NORMAL);
        selected_info_window.print_left(1, "Speed:", A_NORMAL);
        selected_info_window.print_left(2, "Damage:", A_NORMAL);

        selected_info_window.print_right(0, missile.city.name, A_NORMAL);
        selected_info_window.print_right(1, std::to_string(missile.speed), missile.speed > 2 ? COLOR_PAIR(2) : COLOR_PAIR(3));
        selected_info_window.print_right(2, std::to_string(missile.damage), missile.damage > 200 ? COLOR_PAIR(2) : COLOR_PAIR(3));
    }
    else if (game.is_selected_city())
    {
        City &city = game.select_city();

        selected_info_window.print_left(0, "Name:", A_NORMAL);
        selected_info_window.print_left(1, "Hitpoint:", A_NORMAL);
        selected_info_window.print_left(2, "Productivity:", A_NORMAL);
        selected_info_window.print_left(3, "Countdown:", A_NORMAL);
        selected_info_window.print_left(4, "Cruise Storage:", A_NORMAL);

        selected_info_window.print_right(0, city.name, A_NORMAL);
        selected_info_window.print_right(1, std::to_string(city.hitpoint), A_NORMAL);
        selected_info_window.print_right(2, std::to_string(city.productivity), A_NORMAL);
        selected_info_window.print_right(2, std::to_string(city.countdown), A_NORMAL);
        selected_info_window.print_right(2, std::to_string(city.cruise_storage), A_NORMAL);

        if (true) // DEBUG: en_enhanced_radar_II
        {
            int missile_count = 0;
            for (auto missile : game.missile_manager.get_attack_missiles())
            {
                if (missile->get_target() == city.get_position())
                {
                    missile_count++;
                }
            }

            if (missile_count == 0)
            {
                selected_info_window.print_spaces(5, COLOR_PAIR(4));
                selected_info_window.print_left(5, "No missiles targeting the city", COLOR_PAIR(4));
            }
            else if (missile_count < 3)
            {
                selected_info_window.print_spaces(5, COLOR_PAIR(3));
                selected_info_window.print_left(5, std::to_string(missile_count) + " approaching the city", COLOR_PAIR(3));
            }
            else
            {
                selected_info_window.print_spaces(5, COLOR_PAIR(2));
                selected_info_window.print_left(5, std::to_string(missile_count) + " Missiles Approaching !!!", COLOR_PAIR(2));
            }
        }
    }
    else
    {
        selected_info_window.print_left(0, "Nothing Selected Now", A_NORMAL);
    }

    if (game.tech_tree.researching != nullptr)
    {
        tech_info_window.print_left(0, "Researching:", A_NORMAL);
        tech_info_window.print_left(1, "Remaining Time:", A_NORMAL);

        tech_info_window.print_right(0, game.tech_tree.researching->name, A_NORMAL);
        tech_info_window.print_right(1, std::to_string(game.tech_tree.remaining_time), A_NORMAL);
    }
    else
    {
        tech_info_window.print_left(0, "Not Researching", A_NORMAL);
    }

    tech_info_window.print_left(2, "Available:", A_NORMAL);
    tech_info_window.print_left(3, "Researched:", A_NORMAL);

    tech_info_window.print_right(2, "    ", game.tech_tree.available.size() > 0 ? COLOR_PAIR(4) : COLOR_PAIR(3));
    tech_info_window.print_right(2, std::to_string(game.tech_tree.available.size()), game.tech_tree.available.size() > 0 ? COLOR_PAIR(4) : COLOR_PAIR(3));
    tech_info_window.print_right(3, std::to_string(game.tech_tree.researched.size()));

    tech_info_window.print_left(0, "Standard Bomb", A_NORMAL);
    if (game.standard_bomb_counter > 0)
    {
        super_weapon_info_window.print_right(0, "             ", COLOR_PAIR(3));
        tech_info_window.print_right(0, "Remains " + std::to_string(game.standard_bomb_counter), COLOR_PAIR(3));
    }
    else if (game.standard_bomb_counter == 0)
    {
        super_weapon_info_window.print_right(0, "             ", COLOR_PAIR(4));
        super_weapon_info_window.print_left(0, "Ready", COLOR_PAIR(4));
    }
    else
    {
        super_weapon_info_window.print_right(0, "             ", COLOR_PAIR(2));
        super_weapon_info_window.print_right(0, "Not Built", COLOR_PAIR(2));
    }

    if (true) // DEBUG: en_dirty_bomb
    {
        super_weapon_info_window.print_left(1, "Dirty Bomb", A_NORMAL);
        if (game.dirty_bomb_counter > 0)
        {
            super_weapon_info_window.print_right(1, "             ", COLOR_PAIR(3));
            super_weapon_info_window.print_right(1, "Remains " + std::to_string(game.dirty_bomb_counter), COLOR_PAIR(3));
        }
        else if (game.dirty_bomb_counter == 0)
        {
            super_weapon_info_window.print_right(1, "             ", COLOR_PAIR(4));
            super_weapon_info_window.print_right(1, "Ready", COLOR_PAIR(4));
        }
        else
        {
            super_weapon_info_window.print_right(1, "             ", COLOR_PAIR(2));
            super_weapon_info_window.print_right(1, "Not Built", COLOR_PAIR(2));
        }
    }
    if (true) // DEBUG: en_hydrogen_bomb
    {
        super_weapon_info_window.print_left(2, "Hydrogen Bomb", A_NORMAL);
        if (game.hydrogen_bomb_counter > 0)
        {
            super_weapon_info_window.print_right(2, "             ", COLOR_PAIR(3));
            super_weapon_info_window.print_right(2, "Remains" + std::to_string(game.hydrogen_bomb_counter), COLOR_PAIR(3));
        }
        else if (game.hydrogen_bomb_counter == 0)
        {
            super_weapon_info_window.print_right(2, "             ", COLOR_PAIR(4));
            super_weapon_info_window.print_right(2, "Ready", COLOR_PAIR(4));
        }
        else
        {
            super_weapon_info_window.print_right(2, "             ", COLOR_PAIR(2));
            super_weapon_info_window.print_right(2, "Not Built", COLOR_PAIR(2));
        }
    }
    if (true) // DEBUG: en_iron_curtain
    {
        super_weapon_info_window.print_left(3, "Iron Curtain", A_NORMAL);
        if (game.iron_curtain_counter > 40)
        {
            super_weapon_info_window.print_right(3, "             ", COLOR_PAIR(4));
            super_weapon_info_window.print_right(3, "Remains" + std::to_string(game.iron_curtain_counter), COLOR_PAIR(4));
        }
        else if (game.iron_curtain_counter > 0)
        {
            super_weapon_info_window.print_right(3, "             ", COLOR_PAIR(3));
            super_weapon_info_window.print_right(3, "Remains" + std::to_string(game.iron_curtain_counter), COLOR_PAIR(3));
        }
        else
        {
            super_weapon_info_window.print_right(3, "             ", COLOR_PAIR(2));
            super_weapon_info_window.print_right(3, "Not Activated", COLOR_PAIR(2));
        }
    }

    // NOTE: draw operation window
    for (int index = menu.get_offset(); index < menu.get_offset() + menu.get_limit(); index++)
    {
        if (index >= menu.get_items().size())
        {
            break;
        }
        if (index == menu.get_cursor())
        {
            operation_window.print_left(index - menu.get_offset(), menu.get_item(index), A_REVERSE);
        }
        else
        {
            operation_window.print_left(index - menu.get_offset(), menu.get_item(index));
        }
    }

    // NOTE: draw feedback window

    for (int index = 0; index < game.get_feedbacks().size(); index++)
    {
        if (index >= feedback_size.h)
        {
            break;
        }
        feedback_window.print_left(index, game.get_feedbacks().at(game.get_feedbacks().size() - index - 1));
    }
}
