#include <fstream>
#include <memory>
#include "game.h"
#include "loader.h"

Missile::Missile(Position p, Position t, int d, int v)
    : position(p), target(t), progress(MissileProgress::FLYING), damage(d), speed(v)
{
}

MissileDirection Missile::get_direction(void)
{
    if (target.y == position.y)
    {
        if (position.x < target.x)
        {
            return MissileDirection::E;
        }
        if (position.x > target.x)
        {
            return MissileDirection::W;
        }
        return MissileDirection::A;
    }

    if (target.x == position.x)
    {
        if (position.y < target.y)
        {
            return MissileDirection::S;
        }
        if (position.y > target.y)
        {
            return MissileDirection::N;
        }
    }

    if (position.y < target.y)
    {
        if (position.x < target.x)
        {
            return MissileDirection::SE;
        }
        if (position.x > target.x)
        {
            return MissileDirection::SW;
        }
    }
    if (position.y > target.y)
    {
        if (position.x < target.x)
        {
            return MissileDirection::NE;
        }
        if (position.x > target.x)
        {
            return MissileDirection::NW;
        }
    }
    return MissileDirection::U;
}

void Missile::move(void)
{
    for (int step = 0; step < speed; step++)
    {
        move_step();
    }
}

void Missile::move_step(void)
{
    switch (get_direction())
    {
    case MissileDirection::N:
        position.y--;
        return;
    case MissileDirection::NE:
        position.y--;
        position.x++;
        return;
    case MissileDirection::E:
        position.x++;
        return;
    case MissileDirection::SE:
        position.y++;
        position.x++;
        return;
    case MissileDirection::S:
        position.y++;
        return;
    case MissileDirection::SW:
        position.y++;
        position.x--;
        return;
    case MissileDirection::W:
        position.x--;
        return;
    case MissileDirection::NW:
        position.y--;
        position.x--;
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

void Missile::collide(void)
{
    progress = MissileProgress::EXPLODED;
}

AttackMissile::AttackMissile(Position p, City &c, int d, int v)
    : Missile(p, c.get_position(), d, v), city(c)
{
}

void AttackMissile::move_step(void)
{
    Missile::move_step();
}

CruiseMissile::CruiseMissile(Position p, MissilePtr m, int d, int v)
    : Missile(p, m->get_position(), d, v), missile(m)
{
}

void CruiseMissile::move_step(void)
{
    target = missile->get_position();
    Missile::move_step();
    if (get_direction() == MissileDirection::A)
    {
        missile->collide();
        this->collide();
    }
}

City::City(Position p, std::string n, int hp) : position(p), name(n), hitpoint(hp)
{
}

Game::Game(Loader &ldr) : size(ldr.load_size()), cities(ldr.load_cities()), background(ldr.load_background())
{
    cursor = cities[0].position;
    turn = 0;
    // DEBUG: just for testing, remove later
    MissilePtr enemy_missile = std::make_shared<AttackMissile>(AttackMissile({-10, -10}, cities[0], 200, 1));
    missiles.push_back(enemy_missile);
    enemy_missiles.push_back(enemy_missile);
}

void Game::move_cursor(Position dcursor)
{
    if (cursor.y + dcursor.y < 0 || cursor.y + dcursor.y >= size.h)
    {
        return;
    }
    if (cursor.x + dcursor.x < 0 || cursor.x + dcursor.x >= size.w)
    {
        return;
    }
    cursor.y += dcursor.y;
    cursor.x += dcursor.x;
}

void Game::pass_turn(void)
{
    for (auto &missile : enemy_missiles)
    {
        missile->move();
        if (missile->get_direction() == MissileDirection::A && missile->get_progress() == MissileProgress::HIT)
        {
            hit_city(select_city(missile->get_target()), missile->damage);
        }
    }

    for (auto &missile : friendly_missiles)
    {
        missile->move();
        if (missile->get_direction() == MissileDirection::A && missile->get_progress() == MissileProgress::HIT)
        {
            hit_city(select_city(missile->get_target()), missile->damage);
        }
    }

    turn++;
}

City *Game::select_city(void)
{
    for (auto &city : cities)
    {
        if (cursor.y == city.position.y && cursor.x == city.position.x)
        {
            return &city;
        }
    }
    return nullptr;
}

City *Game::select_city(Position p)
{
    for (auto &city : cities)
    {
        if (p.y == city.position.y && p.x == city.position.x)
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

void Game::launch_cruise(void)
{
    City *city = select_city();
    if (city == nullptr)
    {
        return;
    }
    MissilePtr friendly_missile = std::make_shared<CruiseMissile>(CruiseMissile(city->position, enemy_missiles[0], 100, 2));
    missiles.push_back(friendly_missile);
    friendly_missiles.push_back(friendly_missile);
}
