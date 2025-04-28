#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <ctime>
#include "saver.h"
#include "game.h"

#define DEFEND_RADIUS 10

Missile::Missile(int i, Position p, Position t, int d, int v, MissileType tp)
    : id(i), position(p), target(t), is_exploded(false), damage(d), speed(v), type(tp)
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
        set_is_exploded();
        return;

    default:
        return;
    }
}

AttackMissile::AttackMissile(int i, Position p, City &c, int d, int v)
    : Missile(i, p, c.get_position(), d, v, MissileType::ATTACK), city(c)
{
}

void AttackMissile::move_step(void)
{
    Missile::move_step();
}

CruiseMissile::CruiseMissile(int i, Position p, Missile &m, int d, int v, int t_id)
    : Missile(i, p, m.get_position(), d, v, MissileType::CRUISE), missile(m)
{
    target_id = t_id;
}

void CruiseMissile::move_step(void)
{
    target = missile.get_position();
    Missile::move_step();
    if (get_direction() == MissileDirection::A)
    {
        missile.set_is_exploded();
        set_is_exploded();
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
        AttackMissile *attack_missile_ptr = dynamic_cast<AttackMissile *>(attack_missile);
        int distance = abs(attack_missile->get_position().y - c.get_position().y) + abs(attack_missile->get_position().x - c.get_position().x);
        if (distance < target_distance && attack_missile_ptr->is_aimed == false)
        {
            target_distance = distance;
            target_missile = attack_missile;
        }
    }
    if (target_missile == nullptr || target_distance > DEFEND_RADIUS)
    {
        return false;
    }
    AttackMissile *attack_missile_ptr = dynamic_cast<AttackMissile *>(target_missile);
    attack_missile_ptr->is_aimed = true;
    Missile *missile = new CruiseMissile(id++, c.get_position(), *target_missile, d, v, target_missile->id);
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
    for (auto attack_missile : get_attack_missiles())
    {
        if (attack_missile->get_is_exploded())
        {
            for (auto &missile : get_cruise_missiles()) {
                CruiseMissile *cruise_missile = dynamic_cast<CruiseMissile *>(missile);
                if (cruise_missile->target_id == attack_missile->id)
                {
                    auto iter = std::find(missiles.begin(), missiles.end(), cruise_missile);
                    if (iter != missiles.end())
                    {
                        missiles.erase(iter);
                    }
                    delete cruise_missile;
                }
            }
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
int MissileManager::get_process_level(int turn, int hitpoint)
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

int MissileManager::generate_random(int turn, int hitpoint)
{
    int randFactor = get_process_level(turn, hitpoint);
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

void MissileManager::create_attack_wave(int turn, int hitpoint,int difficulty_level)
{
    std::random_device rng;
    int num = turn / inc_turn.at(difficulty_level - 1) + 5;
    for (int i = 0; i < num; i++)
    {
        // randomly generate speed and damage
        int speed = speed_list.at(generate_random(turn, hitpoint));
        int damage = damage_list.at(generate_random(turn, hitpoint));

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
    : position(p), name(n), hitpoint(hp), countdown(0), cruise_storage(0)
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
    TechNode *evacuated_industry = new TechNode("Evacuated Industry", {"City can maintain base production and missile", "storage even after destroyed"}, 10000, 50, {urgent_production});

    // TODO: change effects into more innovative ones
    TechNode *dirty_bomb = new TechNode("Dirty Bomb", {"Allow to launch a new counter-attack missile", "with 50% cost but 75% hit rate"}, 2000, 10, {});
    TechNode *fast_nuke = new TechNode("Fast Nuke", {"Reduce counter-attack missile build-time by 50%"}, 5000, 30, {dirty_bomb});
    TechNode *hydrogen_bomb = new TechNode("Hydrogen Bomb", {"Allow to launch a new counter-attack missile with 500% damage", "at the expense of 50% hit rate and higher building cost"}, 10000, 50, {fast_nuke});

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
    : activated(false), size(s), cursor(cts.at(0).position), turn(0), deposit(0),
      enemy_hitpoint(1000), cities(cts), background(bg), missile_manager(cities), tech_tree()
{
    cursor = cities[0].position;
    turn = 0;
    std::vector<int> inc_turn = {50, 30, 20};
    missile_manager.inc_turn = inc_turn;
    set_difficulty(1);
}

void Game::set_difficulty(int lv)
{
    switch (lv)
    {
        case 1:
        {
            difficulty_level = 1;
            enemy_hitpoint = 1000;
            std::vector<int> sl = {1, 1, 1, 2, 2};
            std::vector<int> dmg = {100, 100, 100, 150, 200};
            missile_manager.speed_list = sl;
            missile_manager.damage_list = dmg;
            deposit = 2000;
            break;
        }
        case 2:
        {

            difficulty_level = 2;
            enemy_hitpoint = 2000;
            std::vector<int> sl = {1, 1, 2, 2, 3};
            std::vector<int> dmg = {100, 100, 200, 200, 200};
            missile_manager.speed_list = sl;
            missile_manager.damage_list = dmg;
            deposit = 1000;
            break;
        }
        case 3:
        {
            difficulty_level = 3;
            enemy_hitpoint = 3000;
            std::vector<int> sl = {1, 2, 2, 3, 3};
            std::vector<int> dmg = {150, 150, 200, 200, 300};
            missile_manager.speed_list = sl;
            missile_manager.damage_list = dmg;
            deposit = 500;
            break;
        }
        default:
        {
            difficulty_level = 1;
            enemy_hitpoint = 1000;
            std::vector<int> sl = {1, 1, 2, 2, 3};
            std::vector<int> dmg = {100, 100, 100, 150, 200};
            missile_manager.speed_list = sl;
            missile_manager.damage_list = dmg;
            break;
        }
    }

}
std::vector<std::string> Game::get_general_info(void)
{
    std::vector<std::string> info;
    info.push_back("Turn: " + std::to_string(turn));
    info.push_back("Deposit: " + std::to_string(deposit));
    info.push_back("Productivity: " + std::to_string(get_productivity()));
    info.push_back("Enemy HP: " + std::to_string(enemy_hitpoint));
    if (true) // DEBUG: en_self_defense_sys
    {
        info.push_back("Self Defense System: ON");
    }
    if (true) // DEBUG: en_enhanced_radar_I
    {

        int missile_count = missile_manager.get_attack_missiles().size();
        if (missile_count == 0)
        {
            info.push_back("No Missiles Approaching");
        }
        else if (missile_count < 5)
        {
            info.push_back(std::to_string(missile_count) + " Missile Approaching");
        }
        else
        {
            info.push_back(std::to_string(missile_count) + " Missiles Approaching !!!");
        }
    }

    return info;
}

std::vector<std::string> Game::get_selected_info(void)
{
    std::vector<std::string> info;
    if (is_selected_missile())
    {
        AttackMissile &missile = dynamic_cast<AttackMissile &>(select_missile());
        info.push_back("Target: " + missile.city.name);
        info.push_back("Speed: " + std::to_string(missile.speed));
        info.push_back("Damage: " + std::to_string(missile.damage));
    }
    else if (is_selected_city())
    {
        City &city = select_city();
        info.push_back("Name: " + city.name);
        info.push_back("Hitpoint: " + std::to_string(city.hitpoint));
        info.push_back("Productivity: " + std::to_string(city.productivity));
        info.push_back("Countdown: " + std::to_string(city.countdown));
        info.push_back("Cruise Storage: " + std::to_string(city.cruise_storage));
        if (true) // DEBUG: en_enhanced_radar_II
        {

            int missile_count = 0;
            for (auto missile : missile_manager.get_attack_missiles())
            {
                if (missile->get_target() == city.get_position())
                {
                    missile_count++;
                }
            }

            if (missile_count == 0)
            {
                info.push_back("No missiles targeting the city");
            }
            else if (missile_count < 3)
            {
                info.push_back(std::to_string(missile_count) + " approaching the city");
            }
            else
            {
                info.push_back(std::to_string(missile_count) + " Missiles Approaching !!!");
            }
        }
    }
    else
    {
        info.push_back("Nothing Selected Now");
    }
    return info;
}

std::vector<std::string> Game::get_tech_info(void) const
{
    std::vector<std::string> info;
    if (tech_tree.researching != nullptr)
    {
        info.push_back("Researching: " + tech_tree.researching->name);
        info.push_back("Remaining Time: " + std::to_string(tech_tree.remaining_time));
    }
    else
    {
        info.push_back("Not Researching");
        info.push_back("");
    }

    info.push_back("Available: " + std::to_string(tech_tree.available.size()));
    info.push_back("Researched: " + std::to_string(tech_tree.researched.size()));

    return info;
}

std::vector<std::string> Game::get_super_weapon_info(void) const
{
    std::vector<std::string> info;
    if (standard_bomb_counter > 0)
    {
        info.push_back("Standard Bomb Build Requires: " + std::to_string(standard_bomb_counter));
    }
    else if (standard_bomb_counter == 0)
    {
        info.push_back("Standard Bomb Ready");
    }
    else
    {
        info.push_back("Standard Bomb Not Built");
    }

    if (true) // DEBUG: en_dirty_bomb
    {
        if (dirty_bomb_counter > 0)
        {
            info.push_back("Dirty Bomb Build Requires: " + std::to_string(dirty_bomb_counter));
        }
        else if (dirty_bomb_counter == 0)
        {
            info.push_back("Dirty Bomb Ready");
        }
        else
        {
            info.push_back("Dirty Bomb Not Built");
        }
    }

    if (true) // DEBUG: en_hydrogen_bomb
    {
        if (hydrogen_bomb_counter > 0)
        {
            info.push_back("Hydrogen Bomb Build Requires: " + std::to_string(hydrogen_bomb_counter));
        }
        else if (hydrogen_bomb_counter == 0)
        {
            info.push_back("Hydrogen Bomb Ready");
        }
        else
        {
            info.push_back("Hydrogen Bomb Not Built");
        }
    }

    if (true) // DEBUG: en_iron_curtain
    {
        if (iron_curtain_activated)
        {
            info.push_back("Iron Curtain Activated");
            info.push_back("Remaining Time: " + std::to_string(iron_curtain_cnt));
        }
        else
        {
            info.push_back("Iron Curtain Not Activated");
        }
    }

    return info;
}

std::vector<std::string> Game::get_feedback_info(void) const
{
    std::vector<std::string> info;
    if (feedbacks.empty())
    {
        return info;
    }
    for (int index = 0; index < 9; index++)
    {
        if (index >= feedbacks.size())
        {
            break;
        }
        info.push_back(feedbacks.at(feedbacks.size() - 1 - index));
    }
    return info;
}

void Game::insert_feedback(const std::string &feedback)
{
    feedbacks.push_back(feedback);
    if (feedbacks.size() > 15)
    {
        feedbacks.erase(feedbacks.begin(), feedbacks.begin() + 5);
    }
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
    missile_manager.remove_missiles();
    missile_manager.update_missiles();

    for (auto missile : missile_manager.get_attack_missiles())
    {
        if (!(missile->type == MissileType::ATTACK))
        {
            continue;
        }
        AttackMissile *attack_missile = dynamic_cast<AttackMissile *>(missile);

        if (attack_missile->get_direction() == MissileDirection::A)
        {
            hit_city(attack_missile->city, attack_missile->damage);
        }
    }

    // TODO: economy parameter tuning
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
            city.cruise_storage = 0;
        }
        deposit += city.productivity;

        if (city.countdown > 0)
        {
            city.countdown--;
            if (city.countdown == 0)
            {
                insert_feedback(city.name + "Cruise Missile Built");
                city.cruise_storage += (en_enhanced_cruise_III ? 2 : 1);
            }
        }
    }

    // NOTE: update global production
    if (standard_bomb_counter > 0)
    {
        standard_bomb_counter--;
    }
    if (dirty_bomb_counter > 0)
    {
        dirty_bomb_counter--;
    }
    if (hydrogen_bomb_counter > 0)
    {
        hydrogen_bomb_counter--;
    }

    // NOTE: update research
    tech_tree.proceed_research();
    check_research();

    // NOTE: iron curtain & self defense system
    check_iron_curtain();
    self_defense();

    if (turn % 40 == 0)
    {
        missile_manager.create_attack_wave(turn, enemy_hitpoint, difficulty_level);
        insert_feedback("New Attack Missile Wave Approaching");
    }
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

bool Game::is_game_over(void) const
{
    if (enemy_hitpoint <= 0)
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
        // TODO: replace 1 with macro MISSILE_SELECT_RANGE
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
        // TODO: replace 1 with macro CITY_SELECT_RANGE
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
        // TODO: replace 1 with macro MISSILE_SELECT_RANGE
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
        // TODO: replace 1 with macro CITY_SELECT_RANGE
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
        insert_feedback(tech_tree.prev_researching->name + " Research Finished");
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
        insert_feedback("Iron Curtain Activated, Damage to " + city.name + " is reduced to 0");
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
    if (!is_selected_city())
    {
        insert_feedback("Please select a city before fixing");
        return;
    }
    City &city = select_city();
    if (!city.is_valid())
    {
        return;
    }
    // TODO: city countdown when fixing
    // TODO: deposit cost when fixing
    insert_feedback("City Fixed, Hitpoint +100");
    city.hitpoint += 100;
}

void Game::build_cruise(void)
{
    if (!is_selected_city())
    {
        insert_feedback("Please select a city before building cruise");
        return;
    }
    City &city = select_city();
    if (!city.is_valid())
    {
        return;
    }
    if (city.countdown > 0)
    {
        insert_feedback("Cruise in building");
        return;
    }
    if (deposit < 200 && !en_enhanced_radar_I)
    {
        insert_feedback("Deposit not enough to build cruise");
        return;
    }
    if (deposit < 100)
    {
        insert_feedback("Deposit not enough to build cruise");
        return;
    }
    deposit -= en_enhanced_cruise_I ? 100 : 200;
    city.countdown = city.cruise_build_time;
}

void Game::launch_cruise(void)
{
    if (!is_selected_city())
    {
        insert_feedback("Please select a city before launching cruise");
        return;
    }
    City &city = select_city();
    if (!city.is_valid())
    {
        return;
    }
    if (city.cruise_storage <= 0)
    {
        insert_feedback("No cruise missile in storage, please build first");
        return;
    }
    if (!missile_manager.create_cruise_missile(city, 100, en_enhanced_cruise_II ? 3 : 2))
    {
        insert_feedback("No attack missile in range");
        return;
    }
    // TODO: city countdown when launching cruise
    insert_feedback("Cruise Missile Launched");
    city.cruise_storage--;
}

void Game::build_standard_bomb(void)
{
    if (standard_bomb_counter == 0)
    {
        insert_feedback("Standard Bomb Already Built");
        return;
    }
    if (standard_bomb_counter > 0)
    {
        insert_feedback("Standard Bomb Already Building");
        return;
    }
    if (deposit < 3000)
    {
        insert_feedback("Deposit not enough to build standard bomb");
        return;
    }

    insert_feedback("Standard Bomb Building Started");
    deposit -= 2000;
    standard_bomb_counter = (en_fast_nuke ? 5 : 10);
}

void Game::launch_standard_bomb(void)
{
    if (standard_bomb_counter == -1)
    {
        insert_feedback("Standard Bomb Not Built");
        return;
    }
    if (standard_bomb_counter > 0)
    {
        insert_feedback("Standard Bomb Not Ready");
        return;
    }

    insert_feedback("Standard Bomb Hit, Enemy Hitpoint Reduced by 50");
    standard_bomb_counter = -1;
    enemy_hitpoint -= 50;
}

void Game::build_dirty_bomb(void)
{
    if (!en_dirty_bomb)
    {
        insert_feedback("Dirty Bomb Not Researched");
        return;
    }
    if (dirty_bomb_counter == 0)
    {
        insert_feedback("Dirty Bomb Already Built");
        return;
    }
    if (dirty_bomb_counter > 0)
    {
        insert_feedback("Dirty Bomb Already Building");
        return;
    }
    if (deposit < 2000)
    {
        insert_feedback("Deposit not enough to build dirty bomb");
        return;
    }

    insert_feedback("Dirty Bomb Building Started");
    deposit -= 1000;
    dirty_bomb_counter = 10;
}

void Game::launch_dirty_bomb(void)
{
    if (dirty_bomb_counter == -1)
    {
        insert_feedback("Dirty Bomb Not Built");
        return;
    }
    if (dirty_bomb_counter > 0)
    {
        insert_feedback("Dirty Bomb Not Ready");
        return;
    }
    dirty_bomb_counter = -1;

    srand(static_cast<unsigned int>(time(nullptr)));
    int rand_factor = rand() % 4;
    if (rand_factor == 0)
    {
        insert_feedback("Dirty Bomb Missed");
        return;
    }
    insert_feedback("Dirty Bomb Hit, Enemy Hitpoint Reduced by 50");
    enemy_hitpoint -= 50;
}

void Game::build_hydrogen_bomb(void)
{
    if (!en_hydrogen_bomb)
    {
        insert_feedback("Hydrogen Bomb Not Researched");
        return;
    }
    if (hydrogen_bomb_counter == 0)
    {
        insert_feedback("Hydrogen Bomb Already Ready");
        return;
    }
    if (hydrogen_bomb_counter > 0)
    {
        insert_feedback("Hydrogen Bomb Already Building");
        return;
    }
    if (deposit < 6000)
    {
        insert_feedback("Deposit not enough to build hydrogen bomb");
        return;
    }
    deposit -= 5000;
    hydrogen_bomb_counter = 20;
}

void Game::launch_hydrogen_bomb(void)
{
    if (hydrogen_bomb_counter == -1)
    {
        insert_feedback("Hydrogen Bomb Not Built");
        return;
    }
    if (hydrogen_bomb_counter > 0)
    {
        insert_feedback("Hydrogen Bomb Not Ready");
        return;
    }
    hydrogen_bomb_counter = -1;

    srand(static_cast<unsigned int>(time(nullptr)));
    int rand_factor = rand() % 2;
    if (rand_factor == 0)
    {
        insert_feedback("Hydrogen Bomb Missed");
        return;
    }
    insert_feedback("Hydrogen Bomb Hit, Enemy Hitpoint Reduced by 500");
    enemy_hitpoint -= 500;
}

void Game::activate_iron_curtain(void)
{
    if (!en_iron_curtain)
    {
        insert_feedback("Iron Curtain Not Researched");
        return;
    }
    if (iron_curtain_activated)
    {
        insert_feedback("Iron Curtain Already Activated");
        return;
    }
    if (deposit < 10000)
    {
        insert_feedback("Deposit not enough to activate iron curtain");
        return;
    }
    insert_feedback("Iron Curtain Activated");
    deposit -= 10000;
    iron_curtain_activated = true;
    iron_curtain_cnt = 30;
}

void Game::check_iron_curtain(void)
{
    if (iron_curtain_activated)
    {
        iron_curtain_cnt--;
        if (iron_curtain_cnt <= 0)
        {
            insert_feedback("Iron Curtain Deactivated");
            iron_curtain_activated = false;
        }
    }
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
            if (missile_manager.create_cruise_missile(city, 100, en_enhanced_cruise_II ? 3 : 2))
            {
                insert_feedback("Self Defense System Activated, Cruise Missile Launched");
            }
        }
    }
}

void MissileManager::reset(void)
{
    for (auto missile : missiles)
    {
        delete missile;
    }
    missiles.clear();
}

void TechTree::reset(void)
{
    researching = nullptr;
    prev_researching = nullptr;
    remaining_time = 0;
    researched.clear();
    available.clear();
}

void Game::reset(void)
{
    activated = false;
    turn = 0;
    deposit = 0;
    enemy_hitpoint = 0;
    cities.clear();
    missile_manager.reset();
    tech_tree.reset();
    feedbacks.clear();
    standard_bomb_counter = -1;
    dirty_bomb_counter = -1;
    hydrogen_bomb_counter = -1;
    iron_curtain_activated = false;
    difficulty_level = 1;
}