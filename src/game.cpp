#include <fstream>
#include "game.h"

City::City(Coordinate p, std::string n, int hp) : position(p), name(n), hitpoint(hp)
{
}

Game::Game(void) : size({18, 98}), cursor({0, 0})
{
    size = {18, 98};
    cursor = {0, 0};
    cities = {
        City({9, 61}, "Shanghai", 100),
        City({8, 52}, "Suzhou", 100),
        City({15, 47}, "Hangzhou", 100),
        City({3, 32}, "Nanjing", 100),
        City({5, 14}, "Hefei", 100),
    };
    // Read Map from background.txt
    std::ifstream file("background.txt");
    std::string line;
    while (std::getline(file, line))
    {
        map.push_back(line);
    }
    file.close();
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
