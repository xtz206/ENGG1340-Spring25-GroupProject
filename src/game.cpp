#include <fstream>
#include "game.h"
#include "loader.h"

City::City(Coordinate p, std::string n, int hp) : position(p), name(n), hitpoint(hp)
{
}

Game::Game(Loader &ldr)
{
    size = {ldr.load_size().first, ldr.load_size().second};
    cities = ldr.load_cities();
    background = ldr.load_background();
}

void Game::move_cursor(int dline, int dcol)
{
    if (cursor.first + dline < 0 || cursor.first + dline >= size.first)
    {
        return;
    }
    if (cursor.second + dcol < 0 || cursor.second + dcol >= size.second)
    {
        return;
    }
    cursor.first += dline;
    cursor.second += dcol;
}

City *Game::select_city(void)
{
    for (auto &city : cities)
    {
        if (cursor.first == city.position.first && cursor.second == city.position.second)
        {
            return &city;
        }
    }
    return nullptr;
}

City *Game::select_city(Coordinate c)
{
    for (auto &city : cities)
    {
        if (c.first == city.position.first && c.second == city.position.second)
        {
            return &city;
        }
    }
    return nullptr;
}
