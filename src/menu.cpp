#include "menu.h"
#include <string>
#include <vector>

Menu::Menu(const std::string &t, const std::vector<std::string> &b, int lim)
    : activated(false), title(t), buttons(b), cursor(0), limit(lim)
{
}

void Menu::move_cursor(int dcursor)
{
    if (cursor + dcursor < 0 || cursor + dcursor >= limit)
    {
        return;
    }
    cursor += dcursor;
}
