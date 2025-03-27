#include <fstream>
#include "game.h"
#include "loader.h"

Missile::Missile(Coordinate p, Coordinate t, int d) : position(p), target(t), progress(MissileProgress::FLYING), damage(d)
{
}

MissileDirection Missile::get_direction(void)
{
    if (target.first == position.first)
    {
        if (position.second < target.second)
        {
            return MissileDirection::E;
        }
        if (position.second > target.second)
        {
            return MissileDirection::W;
        }
        return MissileDirection::A;
    }

    if (target.second == position.second)
    {
        if (position.first < target.first)
        {
            return MissileDirection::S;
        }
        if (position.first > target.first)
        {
            return MissileDirection::N;
        }
    }

    if (position.first < target.first)
    {
        if (position.second < target.second)
        {
            return MissileDirection::SE;
        }
        if (position.second > target.second)
        {
            return MissileDirection::SW;
        }
    }
    if (position.first > target.first)
    {
        if (position.second < target.second)
        {
            return MissileDirection::NE;
        }
        if (position.second > target.second)
        {
            return MissileDirection::NW;
        }
    }
    return MissileDirection::U;
}

void Missile::move(void)
{
    switch (get_direction())
    {
    case MissileDirection::N:
        position.first--;
        return;
    case MissileDirection::NE:
        position.first--;
        position.second++;
        return;
    case MissileDirection::E:
        position.second++;
        return;
    case MissileDirection::SE:
        position.first++;
        position.second++;
        return;
    case MissileDirection::S:
        position.first++;
        return;
    case MissileDirection::SW:
        position.first++;
        position.second--;
        return;
    case MissileDirection::W:
        position.second--;
        return;
    case MissileDirection::NW:
        position.first--;
        position.second--;
        return;

    case MissileDirection::A:
        if (progress == MissileProgress::EXPLODED)
        {
            return;
        }
        if (progress == MissileProgress::HIT)
        {
            progress = MissileProgress::EXPLODED;
            return;
        }
        if (progress == MissileProgress::DESCENDING)
        {
            progress = MissileProgress::HIT;
            return;
        }
        if (progress == MissileProgress::ARRIVED)
        {
            progress = MissileProgress::DESCENDING;
            return;
        }
        if (progress == MissileProgress::FLYING)
        {
            progress = MissileProgress::ARRIVED;
            return;
        }
        return;

    default:
        return;
    }
}

City::City(Coordinate p, std::string n, int hp) : position(p), name(n), hitpoint(hp)
{
}

Game::Game(Loader &ldr) : size(ldr.load_size()), cities(ldr.load_cities()), background(ldr.load_background())
{
    cursor = cities[0].position;
    turn = 0;
    missiles = {Missile({-10, -10}, cities[0].position, 200)};
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

void Game::pass_turn(void)
{
    for (auto &missile : missiles)
    {
        missile.move();
        if (missile.get_direction() == MissileDirection::A && missile.get_progress() == MissileProgress::HIT)
        {
            hit_city(select_city(missile.get_target()), missile.damage);
        }
    }

    turn++;
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

void Game::hit_city(City *city, int damage)
{
    if (city == nullptr)
    {
        return;
    }
    if (city->hitpoint - damage < 0)
    {
        city->hitpoint = 0;
    }
    else
    {
        city->hitpoint -= damage;
    }
}

void Game::fix_city(void)
{
    City *city = select_city();
    if (city == nullptr)
    {
        return;
    }
    city->hitpoint += 100;
}
