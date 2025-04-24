#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

#include "game.h"

#define INF 0x3f3f3f3f
#define DEFEND_RADIUS 5

Missile::Missile(Position p, Position t, int d, int v, MissileType tp)
    : position(p), target(t), progress(MissileProgress::FLYING), damage(d), speed(v), type(tp)
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
    : Missile(p, c.get_position(), d, v, MissileType::ATTACK), city(c)
{
}

void AttackMissile::move_step(void)
{
    Missile::move_step();
}

CruiseMissile::CruiseMissile(Position p, Missile &m, int d, int v)
    : Missile(p, m.get_position(), d, v, MissileType::CRUISE), missile(m)
{
}

void CruiseMissile::move_step(void)
{
    target = missile.get_position();
    Missile::move_step();
    if (get_direction() == MissileDirection::A)
    {
        missile.collide();
        collide();
    }
}
MissileManager::MissileManager(std::vector<City> &cts) : cities(cts) {}

std::vector<Missile *> MissileManager::get_missiles(void)
{
    return missiles;
}

std::vector<Missile *> MissileManager::get_attack_missiles(void)
{
    std::vector<Missile *> attack_missiles;
    for (auto missile : missiles)
    {
        if (missile->type == MissileType::ATTACK)
        {
            attack_missiles.push_back(missile);
        }
    }
    return attack_missiles;
}

std::vector<Missile *> MissileManager::get_cruise_missiles(void)
{
    std::vector<Missile *> cruise_missiles;
    for (auto missile : missiles)
    {
        if (missile->type == MissileType::CRUISE)
        {
            cruise_missiles.push_back(missile);
        }
    }
    return cruise_missiles;
}

void MissileManager::create_attack_missile(Position p, City &c, int d, int v)
{
    Missile *missile = new AttackMissile(p, c, d, v);
    missiles.push_back(missile);
}

bool MissileManager::create_cruise_missile(City &c, int d, int v)
{

    int target_distance = INF;
    Missile *target_missile = nullptr;
    for (auto attack_missile : get_attack_missiles())
    {
        int distance = abs(attack_missile->get_position().y - c.get_position().y) + abs(attack_missile->get_position().x - c.get_position().x);
        if (distance < target_distance)
        {
            target_distance = distance;
            target_missile = attack_missile;
        }
    }
    if (target_missile == nullptr || target_distance > DEFEND_RADIUS)
    {
        return false;
    }
    Missile *missile = new CruiseMissile(c.get_position(), *target_missile, d, v);
    missiles.push_back(missile);
    return true;
}

void MissileManager::update_missiles(void)
{
    for (auto attack_missile : get_attack_missiles())
    {
        attack_missile->move();
    }

    for (auto cruise_missile : get_cruise_missiles())
    {
        cruise_missile->move();
    }
}

void MissileManager::remove_missiles(void)
{
    for (auto cruise_missile : get_cruise_missiles())
    {
        if (cruise_missile->get_progress() == MissileProgress::EXPLODED)
        {
            auto iter = std::find(missiles.begin(), missiles.end(), cruise_missile);
            if (iter != missiles.end())
            {
                missiles.erase(iter);
            }
            delete cruise_missile;
        }
    }

    for (auto &attack_missile : get_attack_missiles())
    {
        if (attack_missile->get_progress() == MissileProgress::EXPLODED)
        {
            auto iter = std::find(missiles.begin(), missiles.end(), attack_missile);
            if (iter != missiles.end())
            {
                missiles.erase(iter);
            }
            delete attack_missile;
        }
    }
}
#define HP_PHASE 200
#define TURN_PHASE 100
int MissileManager::get_process_level(int turn)
{
    int HP_factor = hitpoint / HP_PHASE;
    int turn_factor = turn / TURN_PHASE;
    if (turn_factor > 4)
    {
        turn_factor = 4;
    }
    if (HP_factor > 4)
    {
        HP_factor = 4;
    }
    return (HP_factor + turn_factor) / 2;
}

int MissileManager::generate_random(int turn)
{
    int randFactor = get_process_level(turn);
    std::vector<double> weights = {1.0, 1.0, 1.0, 1.0, 1.0};
    weights[0] += (randFactor == 0) ? 1.0 : 0.0;
    weights[1] += (randFactor == 1) ? 1.0 : 0.0;
    weights[2] += (randFactor == 2) ? 1.0 : 0.0;
    weights[3] += (randFactor == 3) ? 1.0 : 0.0;
    weights[4] += (randFactor == 4) ? 1.0 : 0.0;
    std::random_device rd;
    std::mt19937 mt(rd());
    std::discrete_distribution<> dist(weights.begin(), weights.end());
    return dist(mt);
}

void MissileManager::create_attack_wave(int turn)
{
    std::random_device rng;
    int num = turn / inc_turn[0] + 5;
    for (int i = 0; i < num; i++)
    {
        // randomly generate speed and damage
        int speed = speed_list[generate_random(turn)];
        int damage = damage_list[generate_random(turn)];

        // randomly select a city
        std::uniform_int_distribution<> targetDist(0, cities.size() - 1);
        int target_index = targetDist(rng);

        // randomly generate a pos with in {(0,0),(20,100)}
        std::uniform_int_distribution<> xDist(0, 99);
        std::uniform_int_distribution<> yDist(0, 19);
        std::uniform_int_distribution<> edgeDist(0, 3);

        int edge = edgeDist(rng);
        Position start;
        switch (edge)
        {
        case 0:
            start = Position(yDist(rng), 0);
            break;
        case 1:
            start = Position(yDist(rng), 99);
            break;
        case 2:
            start = Position(0, xDist(rng));
            break;
        case 3:
            start = Position(19, xDist(rng));
            break;
        default:
            start = Position(0, 0);
            break;
        }
        // This if is for debugging purpose, remove later
        if (target_index >= 0 && target_index < cities.size())
        {
            create_attack_missile(start, cities[target_index], damage, speed);
        }
    }
}

City::City(Position p, std::string n, int hp) : position(p), name(n), hitpoint(hp)
{
    deposit = 200;
    productivity = 50 + hitpoint / 10;
}

Game::Game(Size s, std::vector<City> cts, std::vector<std::string> bg)
    : activated(false), size(s), cities(cts), background(bg), missile_manager(cities)
{
    cursor = cities[0].position;
    turn = 0;
    // DEBUG: just for testing, remove later
    std::vector<int> sl = {1, 2, 3, 4, 5};
    std::vector<int> dmg = {100, 150, 200, 250, 300};
    std::vector<int> inc_turn = {50, 30, 10};
    missile_manager.speed_list = sl;
    missile_manager.damage_list = dmg;
    missile_manager.inc_turn = inc_turn;
    missile_manager.hitpoint = 1000;
}

int Game::get_total_deposit(void) const
{
    int total = 0;
    for (auto &city : cities)
    {
        total += city.deposit;
    }
    return total;
}

int Game::get_total_productivity(void) const
{
    int total = 0;
    for (auto &city : cities)
    {
        if (city.hitpoint <= 0)
        {
            continue;
        }
        total += city.productivity;
    }
    return total;
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

    // DEBUG: don't forget to readd city hitting function
    // DONE!
    // TODO: remove cursor
    //       cursor is not a good way to select city
    //       considering use shortcut key to replace cursor in selecting city
    missile_manager.update_missiles();
    missile_manager.remove_missiles();

    for (auto &missile : missile_manager.get_attack_missiles())
    {
        if (missile->get_progress() == MissileProgress::HIT && missile->get_direction() == MissileDirection::A)
        {
            City *city = select_city(missile->get_position());
            if (city == nullptr)
            {
                continue;
            }
            hit_city(city, missile->damage);
        }
    }
    for (auto &city : cities)
    {
        if (city.hitpoint <= 0)
        {
            continue;
        }
        city.deposit += city.productivity;
    }
    if (turn % 40 == 0)
        missile_manager.create_attack_wave(turn);
    turn++;
}

bool Game::is_in_range(Position p1, Position p2, int range) const
{
    if (p1.y == p2.y && p1.x == p2.x)
    {
        return true;
    }
    if (abs(p1.y - p2.y) > range || abs(p1.x - p2.x) > range)
    {
        return false;
    }
    return true;
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
    if (city->deposit < 200)
    {
        return;
    }
    if (missile_manager.create_cruise_missile(*city, 100, 2))
    {
        city->deposit -= 200;
    }
    return;
}
