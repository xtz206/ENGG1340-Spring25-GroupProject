#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>

typedef std::pair<int, int> Coordinate; // NOTE: Coordinate is (y, x) / (line, col) in this game

class City
{
    friend class Game;
    friend class Renderer;

private:
    Coordinate position;
    std::string name;
    int hitpoint;

public:
    City(Coordinate c, std::string n, int hp);
};

class Game
{
private:
    Coordinate size;
    Coordinate cursor;
    std::vector<City> cities;
    std::vector<std::string> map;

public:
    Game(void);

    const Coordinate &get_cursor(void) const { return cursor; };
    const std::vector<std::string> &get_map(void) const { return map; };

    void move_cursor(int dline, int dcol);
    bool is_in_map(Coordinate c) const { return c.first >= 0 && c.first < size.first && c.second >= 0 && c.second < size.second; };
    City *select_city(void);
    City *select_city(Coordinate c);
};

#endif