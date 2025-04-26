#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <ctime>

#include "game.h"

#define DEFEND_RADIUS 10

Missile::Missile(int i, Position p, Position t, int d, int v, MissileType tp)
    : id(i), position(p), target(t), progress(MissileProgress::FLYING), damage(d), speed(v), type(tp)
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

AttackMissile::AttackMissile(int i, Position p, City &c, int d, int v)
    : Missile(i, p, c.get_position(), d, v, MissileType::ATTACK), city(c)
{
}

void AttackMissile::move_step(void)
{
    Missile::move_step();
}

CruiseMissile::CruiseMissile(int i, Position p, Missile &m, int d, int v)
    : Missile(i, p, m.get_position(), d, v, MissileType::CRUISE), missile(m)
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
MissileManager::MissileManager(std::vector<City> &cts) : id(0), cities(cts) {}

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
    Missile *missile = new AttackMissile(id++, p, c, d, v);
    missiles.push_back(missile);
}

bool MissileManager::create_cruise_missile(City &c, int d, int v)
{
    int target_distance = inf;
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
    Missile *missile = new CruiseMissile(id++, c.get_position(), *target_missile, d, v);
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

    for (auto attack_missile : get_attack_missiles())
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

bool MissileManager::city_weight_check(City &c)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int rand_factor = rand();
    if (c.hitpoint <= 0)
    {
        return false;
    }
    if (c.hitpoint > 1000)
    {
        return true;
    }
    if (c.hitpoint > 700)
    {
        if (rand_factor % 8 == 0)
        {
            return false;
        }
        return true;
    }
    if (c.hitpoint > 400)
    {
        if (rand_factor % 4 == 0)
        {
            return false;
        }
        return true;
    }
    if (c.hitpoint > 200)
    {
        if (rand_factor % 3 == 0)
        {
            return false;
        }
        return true;
    }
    else
    {
        if (rand_factor % 2 == 0)
        {
            return false;
        }
        return true;
    }
    return false;
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
        while (!city_weight_check(cities.at(target_index)))
        {
            target_index = targetDist(rng);
        }
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

        create_attack_missile(start, cities.at(target_index), damage, speed);
    }
}

City::City(Position p, std::string n, int hp)
    : position(p), name(n), hitpoint(hp), countdown(0), cruise_num(0)
{
    base_productivity = 50;
    productivity = base_productivity + hitpoint / 10;
}

TechTree::TechTree(void) : researching(nullptr), prev_researching(nullptr), remaining_time(0)
{
    TechNode *enhanced_radar_I = new TechNode("Enhanced Radar I", {"Detect the number of approaching attack missiles"}, 2000, 10, {});
    TechNode *enhanced_radar_II = new TechNode("Enhanced Radar II", {"Detect number of currently targeted missiles of each city"}, 5000, 30, {enhanced_radar_I});
    TechNode *enhanced_radar_III = new TechNode("Enhanced Radar III", {"Display the detailed information of selected missiles"}, 10000, 50, {enhanced_radar_II});

    TechNode *enhanced_cruise_I = new TechNode("Enhanced Cruise I", {"Reduce the cost of cruise by 50%"}, 2000, 10, {});
    TechNode *enhanced_cruise_II = new TechNode("Enhanced Cruise II", {"Increase the speed of cruise by 50%"}, 5000, 30, {enhanced_cruise_I});
    TechNode *enhanced_cruise_III = new TechNode("Enhanced Cruise III", {"Build two cruise at the same time"}, 10000, 50, {enhanced_cruise_II});

    TechNode *self_defense_sys = new TechNode("Self Defense System", {"The city can defense by itself when missiles approaches, ", "if there is missile storage"}, 2000, 10, {enhanced_cruise_III, enhanced_radar_III});

    TechNode *fortress_city = new TechNode("Fortress City", {"City will undertake half of damage"}, 2000, 10, {});
    TechNode *urgent_production = new TechNode("Urgent Production", {"Increase cities's base production by 200%"}, 5000, 30, {fortress_city});
    TechNode *evacuated_industry = new TechNode("Evacuated Industry", {"City can maintain base production and missile storage even after destroyed"}, 10000, 50, {urgent_production});

    // TODO: Change effects into more innovative ones
    TechNode *dirty_bomb = new TechNode("Dirty Bomb", {"Allow to launch a new counter-atk missile with 50% cost but 75% hit rate"}, 2000, 10, {});
    TechNode *fast_nuke = new TechNode("Fast Nuke", {"Reduce counter-atk missile build-time by 50%"}, 5000, 30, {dirty_bomb});
    TechNode *hydrogen_bomb = new TechNode("Hydrogen Bomb", {"Allow to launch a new counter-atk missile with 500% damage", "at the expense of 50% hit rate and higher building cost"}, 10000, 50, {fast_nuke});

    TechNode *iron_curtain = new TechNode("Iron Curtain", {"All your cities will not get damage in next 50 turns"}, 2000, 10, {hydrogen_bomb, evacuated_industry});

    nodes.push_back(enhanced_radar_I);
    nodes.push_back(enhanced_radar_II);
    nodes.push_back(enhanced_radar_III);
    nodes.push_back(enhanced_cruise_I);
    nodes.push_back(enhanced_cruise_II);
    nodes.push_back(enhanced_cruise_III);
    nodes.push_back(self_defense_sys);
    nodes.push_back(fortress_city);
    nodes.push_back(urgent_production);
    nodes.push_back(evacuated_industry);
    nodes.push_back(dirty_bomb);
    nodes.push_back(fast_nuke);
    nodes.push_back(hydrogen_bomb);
    nodes.push_back(iron_curtain);
}

TechTree::~TechTree(void)
{
    for (auto node : nodes)
    {
        delete node;
    }
}

std::vector<std::string> TechTree::get_tech_names(void) const
{
    std::vector<std::string> names;
    for (auto node : nodes)
    {
        names.push_back(node->name);
    }
    return names;
}

void TechTree::start_research(TechNode *node)
{
    if (researching != nullptr || node == nullptr)
    {
        return;
    }
    if (!is_available(node))
    {
        return;
    }
    researching = node;
    remaining_time = node->time;
    available.erase(std::find(available.begin(), available.end(), node));
}

void TechTree::proceed_research(void)
{
    if (researching == nullptr || remaining_time <= 0)
    {
        return;
    }
    remaining_time--;
}

bool TechTree::check_research(void)
{
    if (researching == nullptr)
    {
        return false;
    }
    if (remaining_time > 0)
    {
        return false;
    }
    researched.push_back(researching);
    prev_researching = researching;
    researching = nullptr;
    return true;
}

bool TechTree::check_available(TechNode *node, int deposit) const
{
    if (node->cost > deposit)
    {
        return false;
    }
    for (auto node : node->prerequisites)
    {
        if (std::find(researched.begin(), researched.end(), node) == researched.end())
        {
            return false;
        }
    }
    return true;
}

void TechTree::update_available(int deposit)
{
    for (auto node : nodes)
    {
        if (std::find(available.begin(), available.end(), node) != available.end())
        {
            if (!check_available(node, deposit))
            {
                available.erase(std::find(available.begin(), available.end(), node));
            }
        }
        else
        {
            if (check_available(node, deposit))
            {
                available.push_back(node);
            }
        }
    }
}

Game::Game(Size s, std::vector<City> cts, std::vector<std::string> bg)
    : activated(false), size(s), cities(cts), background(bg), missile_manager(cities),
      tech_tree()
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
    deposit = 0;
}

int Game::get_productivity(void) const
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
    // TODO: add keyboard shortcuts to select city
    missile_manager.update_missiles();
    missile_manager.remove_missiles();

    for (auto missile : missile_manager.get_attack_missiles())
    {
        if (!(missile->type == MissileType::ATTACK))
        {
            continue;
        }
        AttackMissile *attack_missile = static_cast<AttackMissile *>(missile);

        if (attack_missile->get_progress() == MissileProgress::HIT &&
            attack_missile->get_direction() == MissileDirection::A)
        {
            hit_city(attack_missile->city, attack_missile->damage);
        }
    }

    // TODO: Economy Refactor

    // NOTE: update cities production
    for (auto &city : cities)
    {
        if (city.hitpoint > 0)
        {
            city.productivity = city.base_productivity * (en_urgent_production ? 3 : 1) + city.hitpoint / 10;
        }
        else if (en_evacuated_industry)
        {
            city.productivity = city.base_productivity;
        }
        else
        {
            city.hitpoint = 0;
            city.productivity = 0;
            city.countdown = 0;
            city.cruise_num = 0;
        }
        deposit += city.productivity;

        if (city.countdown > 0)
        {
            city.countdown--;
            if (city.countdown == 0)
            {
                city.cruise_num += (en_enhanced_cruise_III ? 2 : 1);
            }
        }
    }

    // NOTE: update global production
    for (int index = 0; index < 3; index++)
    {
        if (countdowns[index] > 0)
        {
            countdowns[index]--;
            if (countdowns[index] == 0)
            {
                attack_missile_num[index]++;
            }
        }
    }

    // NOTE: update research
    tech_tree.proceed_research();
    check_research();

    if (turn % 40 == 0)
        missile_manager.create_attack_wave(turn);
    turn++;

    if (iron_curtain_activated)
    {
        iron_curtain_cnt--;
        if (iron_curtain_cnt <= 0)
        {
            iron_curtain_activated = false;
            iron_curtain_cnt = 30;
        }
    }
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

bool Game::is_game_over(void) const
{
    if (missile_manager.hitpoint <= 0)
    {
        return true;
    }
    for (const auto &city : cities)
    {
        if (city.hitpoint > 0)
        {
            return false;
        }
    }
    return true;
}

bool Game::is_selected_missile(void)
{
    for (auto &missile : missile_manager.get_attack_missiles())
    {
        if (is_in_range(cursor, missile->get_position(), 1))
        {
            return true;
        }
    }
    return false;
}

bool Game::is_selected_city(void)
{
    for (auto &city : cities)
    {
        if (is_in_range(cursor, city.position, 1))
        {
            return true;
        }
    }
    return false;
}

Missile &Game::select_missile(void)
{
    if (!is_selected_missile())
    {
        throw std::runtime_error("No missile selected");
    }
    for (auto &missile : missile_manager.get_attack_missiles())
    {
        if (is_in_range(cursor, missile->get_position(), 1))
        {
            return *missile;
        }
    }
    throw std::runtime_error("No missile selected");
}

City &Game::select_city(void)
{
    if (!is_selected_city())
    {
        throw std::runtime_error("No city selected");
    }

    for (auto &city : cities)
    {
        if (is_in_range(cursor, city.position, 1))
        {
            return city;
        }
    }

    throw std::runtime_error("No city selected");
}

void Game::start_research(TechNode *node)
{
    if (deposit < node->cost)
    {
        return;
    }
    if (!tech_tree.is_available(node))
    {
        return;
    }
    if (tech_tree.researching != nullptr)
    {
        return;
    }
    deposit -= node->cost;
    tech_tree.start_research(node);
}

void Game::check_research(void)
{
    if (tech_tree.check_research())
    {
        finish_research(tech_tree.prev_researching);
    }
    tech_tree.update_available(deposit);
}

void Game::finish_research(TechNode *node)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->name == "Enhanced Radar I")
    {
        en_enhanced_radar_I = true;
    }
    else if (node->name == "Enhanced Radar II")
    {
        en_enhanced_radar_II = true;
    }
    else if (node->name == "Enhanced Radar III")
    {
        en_enhanced_radar_III = true;
    }
    else if (node->name == "Enhanced Cruise I")
    {
        en_enhanced_cruise_I = true;
    }
    else if (node->name == "Enhanced Cruise II")
    {
        en_enhanced_cruise_II = true;
    }
    else if (node->name == "Enhanced Cruise III")
    {
        en_enhanced_cruise_III = true;
    }
    else if (node->name == "Self Defense System")
    {
        en_self_defense_sys = true;
    }
    else if (node->name == "Fortress City")
    {
        en_fortress_city = true;
    }
    else if (node->name == "Urgent Production")
    {
        en_urgent_production = true;
    }
    else if (node->name == "Evacuated Industry")
    {
        en_evacuated_industry = true;
    }
    else if (node->name == "Dirty Bomb")
    {
        en_dirty_bomb = true;
    }
    else if (node->name == "Fast Nuke")
    {
        en_fast_nuke = true;
    }
    else if (node->name == "Hydrogen Bomb")
    {
        en_hydrogen_bomb = true;
    }
    else if (node->name == "Iron Curtain")
    {
        en_iron_curtain = true;
    }
    else
    {
        throw std::runtime_error("Unknown tech node");
    }
}

void Game::hit_city(City &city, int damage)
{
    if (!city.is_valid())
    {
        return;
    }
    if (iron_curtain_activated)
    {
        return;
    }
    if (!city.hitpoint - damage < 0)
    {
        city.hitpoint = 0;
    }
    else
    {
        city.hitpoint -= damage / (en_fortress_city ? 2 : 1);
    }
}

void Game::fix_city(void)
{
    City &city = select_city();
    if (city.is_valid())
    {
        return;
    }
    city.hitpoint += 100;
}

void Game::build_cruise(void)
{
    City &city = select_city();
    if (!city.is_valid())
    {
        return;
    }
    if (deposit < 200 && !en_enhanced_radar_I)
    {
        return;
    }
    if (deposit < 100)
    {
        return;
    }
    deposit -= en_enhanced_cruise_I ? 100 : 200;
    city.countdown = city.cruise_build_time;
}

void Game::launch_cruise(void)
{
    City &city = select_city();
    if (!city.is_valid())
    {
        return;
    }
    if (city.cruise_num <= 0)
    {
        return;
    }
    if (missile_manager.create_cruise_missile(city, 100, en_enhanced_cruise_II ? 3 : 2))
    {
        city.cruise_num--;
        // TODO: city countdown when launching cruise
    }
}

void Game::build_counter_attack(void)
{
    if (deposit < 2000)
    {
        return;
    }
    if (countdowns[0] > 0)
    {
        return;
    }
    deposit -= 2000;
    countdowns[0] = (en_fast_nuke ? 5 : 10);
}

void Game::launch_counter_attack(void)
{
    missile_manager.hitpoint -= 50;
}

void Game::build_dirty_bomb(void)
{
    if (!en_dirty_bomb)
    {
        return;
    }
    if (deposit < 1000)
    {
        return;
    }
    if (countdowns[1] > 0)
    {
        return;
    }
    deposit -= 1000;
    countdowns[1] = 10;
}
void Game::launch_dirty_bomb(void)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int rand_factor = rand() % 4;
    if (rand_factor == 0)
    {
        return;
    }
    missile_manager.hitpoint -= 50;
}

void Game::build_hydrogen_bomb(void)
{
    if (!en_hydrogen_bomb)
    {
        return;
    }
    if (deposit < 5000)
    {
        return;
    }
    if (countdowns[2] > 0)
    {
        return;
    }
    deposit -= 5000;
    countdowns[2] = 20;
}

void Game::launch_hydrogen_bomb(void)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int rand_factor = rand() % 2;
    if (rand_factor == 0)
    {
        return;
    }
    missile_manager.hitpoint -= 500;
}

void Game::activate_iron_curtain(void)
{
    if (!en_iron_curtain)
    {
        return;
    }
    if (iron_curtain_activated)
    {
        return;
    }
    if (deposit < 10000)
    {
        return;
    }
    deposit -= 10000;
    iron_curtain_activated = true;
}

void Game::self_defense(void)
{
    if (!en_self_defense_sys)
    {
        return;
    }
    for (auto &city : cities)
    {
        for (auto missile : missile_manager.get_attack_missiles())
        {
            if (deposit < 1000)
            {
                return;
            }
            if (missile_manager.create_cruise_missile(city, 100, en_enhanced_cruise_II ? 3 : 2))
            {
                deposit -= en_enhanced_cruise_I ? 100 : 200;
            }
        }
    }
}