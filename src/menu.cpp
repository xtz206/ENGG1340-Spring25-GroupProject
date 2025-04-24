#include <string>
#include <vector>

#include "menu.h"

Menu::Menu(const std::string &t, const std::vector<std::string> &b)
    : activated(false), title(t), buttons(b), cursor(0)
{
}

void Menu::move_cursor(int dcursor)
{
    if (cursor + dcursor < 0 || cursor + dcursor >= buttons.size())
    {
        return;
    }
    cursor += dcursor;
}
