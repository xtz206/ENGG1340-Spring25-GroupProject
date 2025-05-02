/**
 * @file game.cpp
 * @brief Implementation of the game mechanics, including missile management, city management,
 *        technology tree, and game progression logic.
 *
 * This file contains the implementation of the core game logic for a missile defense game.
 * It includes classes and methods for managing missiles, cities, the technology tree,
 * and the overall game state. The game involves defending cities from attack missiles
 * using cruise missiles and other technologies while managing resources and progressing
 * through a technology tree.
 *
 * Classes:
 * - Missile: Represents a generic missile with position, target, speed, and damage attributes.
 * - AttackMissile: A subclass of Missile targeting cities.
 * - CruiseMissile: A subclass of Missile targeting other missiles.
 * - MissileManager: Manages all missiles in the game, including creation, movement, and removal.
 * - City: Represents a city with position, hitpoints, productivity, and missile storage.
 * - TechTree: Manages the research and progression of technologies.
 * - Game: Represents the overall game state and logic, including turn progression,
 *         resource management, and game-ending conditions.
 *
 * Key Features:
 * - Missile movement and targeting logic.
 * - City management, including productivity and defense.
 * - Technology tree with research and upgrades.
 * - Game progression, including turn-based mechanics and attack waves.
 * - Resource management and difficulty scaling.
 *
 * Dependencies:
 * - Requires the "game.h" and "saver.h" header files for class definitions and utility functions.
 */

#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include "game.h"
#include "saver.h"

/**
 * @brief Constructor for the Missile class.
 *
 * @param i Unique identifier for the missile.
 * @param p Current position of the missile.
 * @param t Target position of the missile.
 * @param d Damage value of the missile.
 * @param v Speed of the missile.
 * @param tp Type of the missile (Attack or Cruise).
 */
Missile::Missile(int i, Position p, Position t, int d, int v, MissileType tp)
    : id(i), position(p), target(t), is_exploded(false), damage(d), speed(v), type(tp)
{
}

/**
 * @brief Determines the direction of the missile based on its current position and target position.
 *
 * @return MissileDirection: The direction of the missile relative to the target position.
 *
 */
MissileDirection Missile::get_direction(void)
{
    if (target.y == position.y)
    {
        if (position.x < target.x) // Target is to the East
        {
            return MissileDirection::E;
        }
        if (position.x > target.x) // Target is to the West
        {
            return MissileDirection::W;
        }
        return MissileDirection::A; // Missile has arrived
    }

    if (target.x == position.x)
    {
        if (position.y < target.y) // Target is to the South
        {
            return MissileDirection::S;
        }
        if (position.y > target.y) // Target is to the North
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
    return MissileDirection::U; // Unknown direction
}

/**
 * @brief Moves the missile towards its target based on its speed.
 *
 */
void Missile::move(void)
{
    for (int step = 0; step < speed; step++)
    {
        move_step();
    }
}

/**
 * @brief Moves the missile one step towards its target.
 *
 */
void Missile::move_step(void)
{
    // Determine direction and move accordingly
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
        set_is_exploded(); // Missile has arrived, explode
        return;

    default:
        return;
    }
}

/**
 * @brief Constructor for the AttackMissile class.
 *
 * @param i Unique identifier for the missile.
 * @param p Current position of the missile.
 * @param c Target city for the missile.
 * @param d Damage value of the missile.
 * @param v Speed of the missile.
 */
AttackMissile::AttackMissile(int i, Position p, City &c, int d, int v)
    : Missile(i, p, c.get_position(), d, v, MissileType::ATTACK), city(c)
{
}

/**
 * @brief Moves the attack missile one step.
 *
 */
void AttackMissile::move_step(void)
{
    Missile::move_step();
}

/**
 * @brief Constructor for the CruiseMissile class.
 *
 * @param i Unique identifier for the missile.
 * @param p Current position of the missile.
 * @param m Target missile for the cruise missile.
 * @param d Damage value of the missile.
 * @param v Speed of the missile.
 * @param t_id ID of the target missile.
 */
CruiseMissile::CruiseMissile(int i, Position p, Missile &m, int d, int v, int t_id)
    : Missile(i, p, m.get_position(), d, v, MissileType::CRUISE), missile(m)
{
    target_id = t_id;
}

/**
 * @brief Moves the cruise missile one step.
 *
 */
void CruiseMissile::move_step(void)
{
    target = missile.get_position(); // Update target position
    Missile::move_step();
    if (get_direction() == MissileDirection::A) // Check if cruise missile reached target
    {
        missile.set_is_exploded(); // Explode the target missile
        set_is_exploded();         // Explode the cruise missile
    }
}

/**
 * @brief Constructor for the MissileManager class.
 *
 * @param cts Vector of cities in the game.
 */
MissileManager::MissileManager(std::vector<City> &cts) : id(0), cities(cts) {}

/**
 * @brief Gets all missiles managed by the manager.
 *
 * @return std::vector<Missile*>: A vector containing all missiles.
 */
std::vector<Missile *> MissileManager::get_missiles(void)
{
    return missiles;
}

/**
 * @brief Gets all attack missiles managed by the manager.
 *
 * @return std::vector<Missile*>: A vector containing all attack missiles.
 */
std::vector<Missile *> MissileManager::get_attack_missiles(void)
{
    std::vector<Missile *> attack_missiles;
    for (auto missile : missiles) // Iterate through all missiles
    {
        if (missile->type == MissileType::ATTACK) // Check if missile is an attack missile
        {
            attack_missiles.push_back(missile);
        }
    }
    return attack_missiles;
}

/**
 * @brief Gets all cruise missiles managed by the manager.
 *
 * @return std::vector<Missile*>: A vector containing all cruise missiles.
 */
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

/**
 * @brief Creates a new attack missile and adds it to the managed missiles.
 *
 * @param p The starting position of the missile.
 * @param c The target city.
 * @param d The damage the missile will inflict.
 * @param v The speed of the missile.
 */
void MissileManager::create_attack_missile(Position p, City &c, int d, int v)
{
    Missile *missile = new AttackMissile(id++, p, c, d, v);
    missiles.push_back(missile);
}

/**
 * @brief Creates a new cruise missile to defend a city.
 *
 * @param c The city where the missile takes off.
 * @param d The damage the cruise missile can inflict.
 * @param v The velocity of the cruise missile.
 *
 * @return true If a cruise missile is successfully created and assigned to a target.
 * @return false If no suitable target is found or the target is out of the defense radius.
 *
 */
bool MissileManager::create_cruise_missile(City &c, int d, int v)
{
    int target_distance = inf; // Initialize target distance to infinity
    Missile *target_missile = nullptr;
    for (auto attack_missile : get_attack_missiles())
    {
        AttackMissile *attack_missile_ptr = dynamic_cast<AttackMissile *>(attack_missile);                                                      // Cast to AttackMissile pointer
        int distance = abs(attack_missile->get_position().y - c.get_position().y) + abs(attack_missile->get_position().x - c.get_position().x); // Calculate distance to city
        if (distance < target_distance && attack_missile_ptr->is_aimed == false)                                                                // Check if closer and not already aimed
        {
            target_distance = distance;
            target_missile = attack_missile;
        }
    }
    if (target_missile == nullptr || target_distance > 10) // Check if no target or too far
    {
        return false; // No cruise missile created
    }
    AttackMissile *attack_missile_ptr = dynamic_cast<AttackMissile *>(target_missile); // Cast to AttackMissile pointer
    attack_missile_ptr->is_aimed = true;
    Missile *missile = new CruiseMissile(id++, c.get_position(), *target_missile, d, v, target_missile->id); // Create a new cruise missile
    missiles.push_back(missile);
    return true; // Cruise missile created
}

/**
 * @brief Updates the positions of all missiles.
 *
 */
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

/**
 * @brief Removes exploded missiles from the game.
 *
 */
void MissileManager::remove_missiles(void)
{
    for (auto attack_missile : get_attack_missiles())
    {
        if (attack_missile->get_is_exploded())
        {
            for (auto &missile : get_cruise_missiles())
            {
                CruiseMissile *cruise_missile = dynamic_cast<CruiseMissile *>(missile); // Cast to CruiseMissile pointer
                if (cruise_missile->target_id == attack_missile->id)                    // Check if the cruise missile is targeting the exploded attack missile
                {
                    auto iter = std::find(missiles.begin(), missiles.end(), cruise_missile); // Find the cruise missile in the vector
                    if (iter != missiles.end())                                              // Check if the cruise missile was found
                    {
                        missiles.erase(iter);
                    }
                    delete cruise_missile;
                }
            }
            auto iter = std::find(missiles.begin(), missiles.end(), attack_missile); // Find the attack missile in the vector
            if (iter != missiles.end())                                              // Check if the attack missile was found
            {
                missiles.erase(iter);
            }
            delete attack_missile;
        }
    }
}

/**
 * @brief Gets the process level based on the turn and hitpoint.
 *
 * @param turn The current turn number.
 * @param hitpoint The current hitpoint value.
 * @return int: The process level.
 */
int MissileManager::get_process_level(int turn, int hitpoint)
{
    int HP_factor = hitpoint / 200; // Calculate the HP factor
    int turn_factor = turn / 100;   // Calculate the turn factor
    if (turn_factor > 4)            // Limit the turn factor
    {
        turn_factor = 4;
    }
    if (HP_factor > 4) // Limit the HP factor
    {
        HP_factor = 4;
    }
    return (HP_factor + turn_factor) / 2; // Calculate the process level
}

/**
 * @brief Generates a random number based on the turn and hitpoint.
 *
 * @param turn The current turn number.
 * @param hitpoint The current hitpoint value.
 * @return int: A random number.
 */
int MissileManager::generate_random(int turn, int hitpoint)
{
    int randFactor = get_process_level(turn, hitpoint);      // Get the random factor
    std::vector<double> weights = {1.0, 1.0, 1.0, 1.0, 1.0}; // Initialize weights
    weights[0] += (randFactor == 0) ? 1.0 : 0.0;
    weights[1] += (randFactor == 1) ? 1.0 : 0.0;
    weights[2] += (randFactor == 2) ? 1.0 : 0.0;
    weights[3] += (randFactor == 3) ? 1.0 : 0.0;
    weights[4] += (randFactor == 4) ? 1.0 : 0.0;
    std::random_device rd;
    std::mt19937 mt(rd());                                             // Create a Mersenne Twister engine
    std::discrete_distribution<> dist(weights.begin(), weights.end()); // Create a discrete distribution
    return dist(mt);                                                   // Generate a random number
}

/**
 * @brief Sets the difficulty level for the missile manager by adjusting the speed and damage of missiles.
 *
 * @param lv Difficulty level (1-3).
 *
 */
void MissileManager::set_difficulty(int lv)
{
    switch (lv)
    {
    case 3:
        speed_list = {1, 1, 1, 2, 2};
        damage_list = {100, 100, 100, 150, 200};
        break;
    case 2:
        speed_list = {1, 1, 2, 2, 3};
        damage_list = {100, 100, 200, 200, 200};
        break;

    case 1:
    default:
        speed_list = {1, 2, 2, 3, 3};
        damage_list = {150, 150, 200, 200, 300};
        break;
    }
}

/**
 * @brief Checks the weight of a city based on its hitpoint.
 *
 * @param c The city to check.
 * @return true If the city is valid, false otherwise.
 */
bool MissileManager::city_weight_check(City &c)
{
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator
    int rand_factor = rand();                        // Generate a random factor
    if (c.hitpoint <= 0)                             // Check if the city is destroyed
    {
        return false; // City is not valid
    }
    if (c.hitpoint > 1000) // Check if the city has high hitpoints
    {
        return true; // City is valid
    }
    if (c.hitpoint > 700) // Check if the city has medium-high hitpoints
    {
        if (rand_factor % 8 == 0)
        {
            return false;
        }
        return true;
    }
    if (c.hitpoint > 400) // Check if the city has medium hitpoints
    {
        if (rand_factor % 4 == 0)
        {
            return false;
        }
        return true;
    }
    if (c.hitpoint > 200) // Check if the city has low-medium hitpoints
    {
        if (rand_factor % 3 == 0)
        {
            return false;
        }
        return true;
    }
    else // City has low hitpoints
    {
        if (rand_factor % 2 == 0)
        {
            return false;
        }
        return true;
    }
    return false;
}

/**
 * @brief Creates a wave of attack missiles.
 *
 * @param turn The current turn number.
 * @param hitpoint The current hitpoint value.
 * @param difficulty_level The difficulty level of the game.
 */
void MissileManager::create_attack_wave(int turn, int hitpoint, int difficulty_level)
{
    std::random_device rng;                                 // Create a random device
    int num = turn / inc_turn.at(difficulty_level - 1) + 5; // Calculate the number of missiles
    for (int i = 0; i < num; i++)                           // Iterate to create missiles
    {
        // randomly generate speed and damage
        int speed = speed_list.at(generate_random(turn, hitpoint));
        int damage = damage_list.at(generate_random(turn, hitpoint));

        // randomly select a city
        std::uniform_int_distribution<> targetDist(0, cities.size() - 1); // Create a uniform distribution
        int target_index = targetDist(rng);                               // Generate a random index
        while (!city_weight_check(cities.at(target_index)))               // Check if the city is valid
        {
            target_index = targetDist(rng); // Generate a new random index
        }
        // randomly generate a pos with in {(y,x) | 0 < y <= size.h, 0 < x <= size.w}
        std::uniform_int_distribution<> xDist(0, size.w + 1);
        std::uniform_int_distribution<> yDist(0, size.h + 1);
        std::uniform_int_distribution<> edgeDist(0, 3);

        int edge = edgeDist(rng); // Generate a random edge
        Position start;           // Declare start position
        switch (edge)             // Determine start position based on edge
        {
        case 0:
            start = Position(yDist(rng), 0); // Left edge
            break;
        case 1:
            start = Position(yDist(rng), size.w + 1); // Right edge
            break;
        case 2:
            start = Position(0, xDist(rng)); // Top edge
            break;
        case 3:
            start = Position(size.h + 1, xDist(rng)); // Bottom edge
            break;
        default:
            start = Position(0, 0); // Default position
            break;
        }

        create_attack_missile(start, cities.at(target_index), damage, speed); // Create and add the attack missile
    }
}

/**
 * @brief Constructor for the City class.
 *
 * @param p Position of the city.
 * @param n Name of the city.
 * @param hp Hitpoints of the city.
 */
City::City(Position p, const std::string &n, int hp)
    : position(p), name(n), hitpoint(hp), countdown(0), cruise_storage(0)
{
    base_productivity = 50;
    productivity = base_productivity + hitpoint / 10;
}

/**
 * @brief Constructs the TechTree and initializes all technology nodes.
 * Adds enhanced radar systems, cruise systems, defense mechanisms and strategic weapons to the technology tree.
 */
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

/**
 * @brief Destructor for the TechTree class.
 * Deallocates memory used by all technology tree nodes.
 */
TechTree::~TechTree(void)
{
    for (auto node : nodes)
    {
        delete node;
    }
}

/**
 * @brief Retrieves names of all available technologies.
 * @return std::vector<std::string> List containing names of all tech nodes.
 */
std::vector<std::string> TechTree::get_tech_names(void) const
{
    std::vector<std::string> names; // Initialize a vector of strings
    for (auto node : nodes)
    {
        names.push_back(node->name); // Add the node's name to the vector
    }
    return names;
}

/**
 * @brief Initiates research on specified technology node.
 * @param node Pointer to the TechNode to start researching.
 */
void TechTree::start_research(TechNode *node)
{
    if (researching != nullptr || node == nullptr) // Check if already researching or node is null
    {
        return;
    }
    if (!is_available(node))
    {
        return;
    }
    researching = node;
    remaining_time = node->time;
    available.erase(std::find(available.begin(), available.end(), node)); // Remove the node from available
}

/**
 * @brief Advances the current research progress.
 * Reduces remaining research time by one unit per call.
 */
void TechTree::proceed_research(void)
{
    if (researching == nullptr || remaining_time <= 0) // Check if not researching or research is complete
    {
        return;
    }
    remaining_time--;
}

/**
 * @brief Checks research completion status.
 * Moves completed research to researched list when finished.
 * @return bool True if research completed successfully，false otherwise.
 */
bool TechTree::check_research(void)
{
    if (researching == nullptr) // Check if not researching
    {
        return false; // Research is not complete
    }
    if (remaining_time > 0) // Check if research is still in progress
    {
        return false;
    }
    researched.push_back(researching); // Add the node to researched
    prev_researching = researching;    // Set the previous researching node
    researching = nullptr;             // Reset the researching node
    return true;                       // Research is complete
}

/**
 * @brief Validates node research availability.
 * @param node TechNode to validate.
 * @param deposit Current available resources.
 * @return bool True if node meets cost and prerequisite requirements.
 */
bool TechTree::check_available(TechNode *node, int deposit) const
{
    if (node->cost > deposit)
    {
        return false; // Node is not available
    }
    for (auto node : node->prerequisites)
    {
        if (std::find(researched.begin(), researched.end(), node) == researched.end()) // Check if the prerequisite is not researched
        {
            return false;
        }
    }
    return true; // Node is available
}

/**
 * @brief Updates available technologies list.
 * Synchronizes available nodes with current game state.
 * @param deposit Current resource amount for availability checks.
 */
void TechTree::update_available(int deposit)
{
    for (auto node : nodes)
    {
        if (std::find(available.begin(), available.end(), node) != available.end()) // Check if the node is in available
        {
            if (!check_available(node, deposit)) // Check if the node is not available
            {
                available.erase(std::find(available.begin(), available.end(), node));
            }
        }
        else // The node is not in available
        {
            if (check_available(node, deposit)) // Check if the node is available
            {
                available.push_back(node);
            }
        }
    }
}

/**
 * @brief Sets the difficulty level for the game by adjusting the initial deposit and enemy hitpoint.
 *
 * @param lv Difficulty level (1-3).
 */
void Game::set_difficulty(int lv)
{
    missile_manager.set_difficulty(lv);
    switch (lv) // Determine difficulty based on input
    {
    case 1: // Easy difficulty
    default:
    {
        difficulty_level = 1;
        enemy_hitpoint = 1000;
        deposit = 2000;
        break;
    }
    case 2: // Medium difficulty
    {
        difficulty_level = 2;
        enemy_hitpoint = 2000;
        deposit = 1000;
        break;
    }
    case 3: // Hard difficulty
    {
        difficulty_level = 3;
        enemy_hitpoint = 3000;
        deposit = 500;
        break;
    }
    }
}

void Game::insert_feedback(const AttrString &feedback)
{
    feedbacks.push_back(feedback);
    if (feedbacks.size() > 15)
    {
        feedbacks.erase(feedbacks.begin(), feedbacks.begin() + 5); // Erase the first 5 feedback messages
    }
}

/**
 * @brief Calculates total industrial output from operational cities. Excludes destroyed cities.
 * @return int Sum of active cities' productivity values
 */
int Game::get_productivity(void) const
{
    int total = 0;
    for (auto &city : cities)
    {
        if (city.hitpoint <= 0) // Check if the city is destroyed
        {
            continue; // Skip the city
        }
        total += city.productivity;
    }
    return total;
}

/**
 * @brief Adjusts cursor position with boundary constraints. Ensures position stays within
 *        game board dimensions.
 * @param dcursor Position delta for movement
 */
void Game::move_cursor(Position dcursor)
{
    if (cursor.y + dcursor.y < 0 || cursor.y + dcursor.y >= size.h) // Check if the new Y position is out of bounds
    {
        return;
    }
    if (cursor.x + dcursor.x < 0 || cursor.x + dcursor.x >= size.w) // Check if the new X position is out of bounds
    {
        return;
    }
    cursor.y += dcursor.y; // Update the Y position of the cursor
    cursor.x += dcursor.x; // Update the X position of the cursor
}

/**
 * @brief Moves the cursor to a specific city based on its index.
 * @param index Index of the city to move the cursor to
 */
void Game::move_cursor_to_city(int index)
{
    if (index < 0 || index >= cities.size())
    {
        return;
    }
    cursor = cities.at(index).get_position();
}

/**
 * @brief Advances game state by one turn. Handles missile updates, city production, research
 *        progress, and periodic attack waves.
 */
void Game::pass_turn(void)
{
    missile_manager.remove_missiles(); // Remove exploded missiles
    missile_manager.update_missiles(); // Update missile positions

    for (auto missile : missile_manager.get_attack_missiles())
    {
        if (!(missile->type == MissileType::ATTACK)) // Check if the missile is an attack missile
        {
            continue;
        }
        AttackMissile *attack_missile = dynamic_cast<AttackMissile *>(missile); // Cast to AttackMissile pointer

        if (attack_missile->get_direction() == MissileDirection::A) // Check if the missile has reached its target
        {
            hit_city(attack_missile->city, attack_missile->damage); // Hit the city with the missile
        }
    }

    // TODO: economy parameter tuning
    // NOTE: update cities production
    for (auto &city : cities)
    {
        if (city.hitpoint > 0) // Check if the city is not destroyed
        {
            city.productivity = city.base_productivity * (en_urgent_production ? 3 : 1) + city.hitpoint / 10; // Update city productivity
        }
        else if (en_evacuated_industry) // Check if evacuated industry is enabled
        {
            city.productivity = city.base_productivity; // Maintain base productivity
        }
        else // City is destroyed
        {
            city.hitpoint = 0;
            city.productivity = 0;
            city.countdown = 0;
            city.cruise_storage = 0;
        }
        deposit += city.productivity;

        if (city.countdown > 0) // Check if the city is building a cruise missile
        {
            city.countdown--;
            if (city.countdown == 0) // Check if the countdown is complete
            {
                insert_feedback(city.name + " Cruise Missile Built");
                city.cruise_storage += (en_enhanced_cruise_III ? 2 : 1);
            }
        }
    }

    // NOTE: update global production
    if (standard_bomb_counter > 0) // Check if standard bomb is building
    {
        standard_bomb_counter--;
    }
    if (dirty_bomb_counter > 0) // Check if dirty bomb is building
    {
        dirty_bomb_counter--;
    }
    if (hydrogen_bomb_counter > 0) // Check if hydrogen bomb is building
    {
        hydrogen_bomb_counter--;
    }

    // NOTE: update research
    tech_tree.proceed_research();
    check_research(); // Check if research is complete

    // NOTE: iron curtain & self defense system
    check_iron_curtain(); // Check if iron curtain is active
    self_defense();       // Activate self defense system

    if (turn % 40 == 0) // Check if it's time to launch a new attack wave
    {
        missile_manager.create_attack_wave(turn, enemy_hitpoint, difficulty_level);
        insert_feedback("New Attack Missile Wave Approaching");
    }
    turn++;
}

/**
 * @brief Verifies proximity between two positions. Uses chessboard distance metric.
 * @param p1 First position coordinates
 * @param p2 Second position coordinates
 * @param range Maximum allowed distance
 * @return bool True if within range, false otherwise
 */
bool Game::is_in_range(Position p1, Position p2, int range) const
{
    if (p1.y == p2.y && p1.x == p2.x) // Check if the positions are the same
    {
        return true; // Positions are in range
    }
    if (abs(p1.y - p2.y) > range || abs(p1.x - p2.x) > range) // Check if the positions are too far apart
    {
        return false; // Positions are not in range
    }
    return true;
}

/**
 * @brief Determines game termination conditions. Checks enemy destruction or complete city loss.
 * @return bool True if game should end, false otherwise
 */
bool Game::check_game_over(void)
{
    // TODO: fine tune score params
    if (enemy_hitpoint <= 0) // Check if the enemy has been defeated
    {
        for (const auto &city : cities)
        {
            if (city.hitpoint > 0)
            {
                score += 500;
            }
        }
        score += (100 - turn / 10);
        return true; // Game win
    }

    for (const auto &city : cities)
    {
        if (city.hitpoint > 0) // Check if any city is still alive
        {
            return false; // Game continues
        }
    }

    score -= 1000;
    return true; // Game lose
}

/**
 * @brief Verifies missile selection state. Checks cursor proximity to active missiles.
 * @return bool True if any missile is selected
 */
bool Game::is_selected_missile(void)
{
    for (auto &missile : missile_manager.get_attack_missiles())
    {
        if (is_in_range(cursor, missile->get_position(), 1))
        {
            return true; // Missile is selected
        }
    }
    return false; // No missile is selected
}

/**
 * @brief Verifies city selection state. Checks cursor proximity to city positions.
 * @return bool True if any city is selected
 */
bool Game::is_selected_city(void)
{
    for (auto &city : cities)
    {
        if (is_in_range(cursor, city.position, 1))
        {
            return true; // City is selected
        }
    }
    return false; // No city is selected
}

/**
 * @brief Retrieves selected missile reference. Requires valid selection state.
 * @return Missile& Reference to selected missile
 * @throws std::runtime_error When no missile is selected
 */
Missile &Game::select_missile(void)
{
    if (!is_selected_missile())
    {
        throw std::runtime_error("No missile selected"); // Throw error if no missile selected
    }
    for (auto &missile : missile_manager.get_attack_missiles())
    {
        if (is_in_range(cursor, missile->get_position(), 1)) // Check if cursor is in range of missile
        {
            return *missile; // Return selected missile
        }
    }
    throw std::runtime_error("No missile selected"); // Throw error if no missile selected
}

/**
 * @brief Retrieves selected city reference. Requires valid selection state.
 * @return City& Reference to selected city
 * @throws std::runtime_error When no city is selected
 */
City &Game::select_city(void)
{
    if (!is_selected_city())
    {
        throw std::runtime_error("No city selected"); // Throw error if no city selected
    }

    for (auto &city : cities)
    {
        if (is_in_range(cursor, city.position, 1)) // Check if cursor is in range of city
        {
            return city; // Return selected city
        }
    }

    throw std::runtime_error("No city selected"); // Throw error if no city selected
}

/**
 * @brief Initiates technology research process. Validates resource and prerequisite conditions.
 * @param node Target TechNode to research
 */
void Game::start_research(TechNode *node)
{
    if (deposit < node->cost) // Check if deposit is sufficient
    {
        return; // Do nothing if not sufficient
    }
    if (!tech_tree.is_available(node)) // Check if node is available
    {
        return; // Do nothing if not available
    }
    if (tech_tree.researching != nullptr) // Check if already researching
    {
        return; // Do nothing if already researching
    }
    deposit -= node->cost;
    tech_tree.start_research(node);
}

/**
 * @brief Finalizes completed research and applies technology effects. Updates availability.
 */
void Game::check_research(void)
{
    if (tech_tree.check_research()) // Check if research is complete
    {
        insert_feedback(tech_tree.prev_researching->name + " Research Finished");
        finish_research(tech_tree.prev_researching);
    }
    tech_tree.update_available(deposit);
}

/**
 * @brief Activates technology-specific game mechanics. Configures system parameters based
 *        on researched technology.
 * @param node Completed TechNode reference
 */
void Game::finish_research(TechNode *node)
{
    if (node == nullptr)
    {
        return;
    }
    if (node->name == "Enhanced Radar I") // Check if node is Enhanced Radar I
    {
        score += 10;
        en_enhanced_radar_I = true;
    }
    else if (node->name == "Enhanced Radar II") // Similar as above
    {
        score += 20;
        en_enhanced_radar_II = true;
    }
    else if (node->name == "Enhanced Radar III")
    {
        score += 30;
        en_enhanced_radar_III = true;
    }
    else if (node->name == "Enhanced Cruise I")
    {
        score += 10;
        en_enhanced_cruise_I = true;
    }
    else if (node->name == "Enhanced Cruise II")
    {
        score += 20;
        en_enhanced_cruise_II = true;
    }
    else if (node->name == "Enhanced Cruise III")
    {
        score += 30;
        en_enhanced_cruise_III = true;
    }
    else if (node->name == "Self Defense System")
    {
        score += 50;
        en_self_defense_sys = true;
    }
    else if (node->name == "Fortress City")
    {
        score += 10;
        en_fortress_city = true;
    }
    else if (node->name == "Urgent Production")
    {
        score += 20;
        en_urgent_production = true;
    }
    else if (node->name == "Evacuated Industry")
    {
        score += 30;
        en_evacuated_industry = true;
    }
    else if (node->name == "Dirty Bomb")
    {
        score += 10;
        en_dirty_bomb = true;
    }
    else if (node->name == "Fast Nuke")
    {
        score += 20;
        en_fast_nuke = true;
    }
    else if (node->name == "Hydrogen Bomb")
    {
        score += 30;
        en_hydrogen_bomb = true;
    }
    else if (node->name == "Iron Curtain")
    {
        score += 50;
        en_iron_curtain = true;
    }
    else // Unknown tech node
    {
        throw std::runtime_error("Unknown tech node"); // Throw error if unknown tech node
    }
}

/**
 * @brief Applies damage to target city. Implements damage reduction mechanics.
 * @param city Target city reference
 * @param damage Base damage value before modifiers
 */
void Game::hit_city(City &city, int damage)
{
    // TODO: score/casualty param fine tune
    if (iron_curtain_counter >= 0)
    {
        insert_feedback("Iron Curtain Activated, " + city.name + " Not Damaged");
        return;
    }
    damage = en_self_defense_sys ? damage / 2 : damage;
    if (damage > city.hitpoint && city.hitpoint > 0)
    {
        insert_feedback(AttrString(city.name + " Destroyed by Attack Missile!", COLOR_PAIR(2)));
        city.hitpoint = 0;
        score -= 50;
        casualty += 100;
    }
    else
    {
        insert_feedback(AttrString(city.name + " Hit by Attack Missile, HP -" + std::to_string(damage / (en_fortress_city ? 2 : 1)), COLOR_PAIR(2)));
        city.hitpoint -= damage;
        score -= 20;
        casualty += 30;
    }
}

/**
 * @brief Repairs selected city infrastructure. Requires valid city selection.
 */
void Game::fix_city(void)
{
    if (!is_selected_city())
    {
        insert_feedback("Please select a city before fixing");
        return;
    }
    City &city = select_city();
    if (deposit < 5000)
    {
        insert_feedback("Deposit not enough (5000) to fix city");
        return;
    }
    insert_feedback("City Fixed, HP +100");
    city.hitpoint += 500;
}

/**
 * @brief Initiates cruise missile production in selected city. Validates resources and
 *        production capacity.
 */
void Game::build_cruise(void)
{
    if (!is_selected_city())
    {
        insert_feedback("Please select a city before building cruise");
        return;
    }
    City &city = select_city();
    if (city.countdown > 0) // Check if already building cruise
    {
        insert_feedback("Cruise in building");
        return;
    }
    if (deposit < 200 && !en_enhanced_radar_I)
    {
        insert_feedback("Deposit not enough(200) to build cruise");
        return;
    }
    if (deposit < 100) // Check if deposit is sufficient
    {
        insert_feedback("Deposit not enough(100) to build cruise");
        return;
    }
    deposit -= en_enhanced_cruise_I ? 100 : 200;
    city.countdown = 5; // Start the build countdown
}

/**
 * @brief Launches cruise missile from selected city. Requires available missiles and
 *        valid targets.
 */
void Game::launch_cruise(void)
{
    if (!is_selected_city())
    {
        insert_feedback("Please select a city before launching cruise");
        return;
    }
    City &city = select_city();
    if (city.cruise_storage <= 0) // Check if cruise storage is empty
    {
        insert_feedback("No cruise missile in storage, please build first");
        return;
    }
    if (!missile_manager.create_cruise_missile(city, 100, en_enhanced_cruise_II ? 3 : 2)) // Try to create cruise missile
    {
        insert_feedback("No attack missile in range");
        return;
    }
    // TODO: city countdown when launching cruise
    insert_feedback("Cruise Missile Launched");
    city.cruise_storage--;
}

/**
 * @brief Initiates standard nuclear weapon construction. Validates resource requirements.
 */
void Game::build_standard_bomb(void)
{
    if (standard_bomb_counter == 0) // Check if standard bomb is already built
    {
        insert_feedback("Standard Bomb Already Built");
        return;
    }
    if (standard_bomb_counter > 0) // Check if standard bomb is already building
    {
        insert_feedback("Standard Bomb Already Building");
        return;
    }
    if (deposit < 3000) // Check if deposit is sufficient
    {
        insert_feedback("Deposit not enough(3000) to build standard bomb");
        return;
    }

    insert_feedback("Standard Bomb Building Started");
    deposit -= 2000;
    standard_bomb_counter = (en_fast_nuke ? 5 : 10); // Start the build counter
}

/**
 * @brief Deploys standard nuclear strike. Applies guaranteed damage on successful launch.
 */
void Game::launch_standard_bomb(void)
{
    if (standard_bomb_counter == -1) // Check if standard bomb is not built
    {
        insert_feedback("Standard Bomb Not Built");
        return;
    }
    if (standard_bomb_counter > 0) // Check if standard bomb is not ready
    {
        insert_feedback("Standard Bomb Not Ready");
        return;
    }

    insert_feedback("Standard Bomb Hit, Enemy Hitpoint Reduced by 50");
    standard_bomb_counter = -1; // Set counter to -1 to indicate bomb has been used
    enemy_hitpoint -= 50;
    score += 20;
}

/**
 * @brief Constructs a dirty bomb. Handles research checks, resource validation, and build status tracking.
 * Deducts 1000 deposit and initializes 10-turn build timer when conditions are met.
 */
void Game::build_dirty_bomb(void)
{
    if (!en_dirty_bomb) // Check if dirty bomb is not researched
    {
        insert_feedback("Dirty Bomb Not Researched");
        return;
    }
    if (dirty_bomb_counter == 0) // Check if dirty bomb is already built
    {
        insert_feedback("Dirty Bomb Already Built");
        return;
    }
    if (dirty_bomb_counter > 0) // Check if dirty bomb is already building
    {
        insert_feedback("Dirty Bomb Already Building");
        return;
    }
    if (deposit < 2000) // Check if deposit is sufficient
    {
        insert_feedback("Deposit not enough(2000) to build dirty bomb");
        return;
    }

    insert_feedback("Dirty Bomb Building Started");
    deposit -= 1000;
    dirty_bomb_counter = 10; // Start the build counter
}

/**
 * @brief Deploys constructed dirty bomb with probabilistic targeting.
 * Verifies build completion status, implements 25% miss chance through RNG, applies 50 damage
 * on successful hit. Transitions bomb state to expended after deployment.
 */
void Game::launch_dirty_bomb(void)
{
    if (dirty_bomb_counter == -1) // Check if dirty bomb is not built
    {
        insert_feedback("Dirty Bomb Not Built");
        return;
    }
    if (dirty_bomb_counter > 0) // Check if dirty bomb is not ready
    {
        insert_feedback("Dirty Bomb Not Ready");
        return;
    }
    dirty_bomb_counter = -1; // Set counter to -1 to indicate bomb has been used

    srand(static_cast<unsigned int>(time(nullptr))); // Seed random number generator
    int rand_factor = rand() % 4;                    // Generate random factor
    if (rand_factor == 0)                            // Check if bomb misses
    {
        insert_feedback("Dirty Bomb Missed");
        return;
    }
    insert_feedback("Dirty Bomb Hit, Enemy Hitpoint Reduced by 50");
    enemy_hitpoint -= 50;
    score += 20;
}

/**
 * @brief Initiates hydrogen bomb production with multi-phase validation.
 * Requires completed research, available construction slot, and 5000 deposit. Activates
 * 20-turn build timer upon successful validation.
 */
void Game::build_hydrogen_bomb(void)
{
    if (!en_hydrogen_bomb) // Check if hydrogen bomb is not researched
    {
        insert_feedback("Hydrogen Bomb Not Researched");
        return;
    }
    if (hydrogen_bomb_counter == 0) // Check if hydrogen bomb is already ready
    {
        insert_feedback("Hydrogen Bomb Already Ready");
        return;
    }
    if (hydrogen_bomb_counter > 0) // Check if hydrogen bomb is already building
    {
        insert_feedback("Hydrogen Bomb Already Building");
        return;
    }
    if (deposit < 6000) // Check if deposit is sufficient
    {
        insert_feedback("Deposit not enough(6000) to build hydrogen bomb");
        return;
    }
    deposit -= 5000;
    hydrogen_bomb_counter = 20; // Start the build counter
}

/**
 * @brief Executes hydrogen bomb deployment sequence.
 * Confirms build readiness, calculates 50% hit probability, applies 500 damage on hit.
 * Resets bomb state to unavailable post-deployment regardless of outcome.
 */
void Game::launch_hydrogen_bomb(void)
{
    if (hydrogen_bomb_counter == -1) // Check if hydrogen bomb is not built
    {
        insert_feedback("Hydrogen Bomb Not Built");
        return;
    }
    if (hydrogen_bomb_counter > 0) // Check if hydrogen bomb is not ready
    {
        insert_feedback("Hydrogen Bomb Not Ready");
        return;
    }
    hydrogen_bomb_counter = -1; // Set counter to -1 to indicate bomb has been used

    srand(static_cast<unsigned int>(time(nullptr))); // Seed random number generator
    int rand_factor = rand() % 2;                    // Generate random factor
    if (rand_factor == 0)                            // Check if bomb misses
    {
        insert_feedback("Hydrogen Bomb Missed");
        return;
    }
    insert_feedback("Hydrogen Bomb Hit, Enemy Hitpoint Reduced by 500");
    enemy_hitpoint -= 500;
    score += 50;
}

/**
 * @brief Engages temporary invulnerability protection system.
 * Validates research completion and 10000 deposit payment. Activates 30-turn protection
 * duration counter when conditions met.
 */
void Game::activate_iron_curtain(void)
{
    if (!en_iron_curtain) // Check if iron curtain is not researched
    {
        insert_feedback("Iron Curtain Not Researched");
        return;
    }
    if (iron_curtain_counter >= 0) // Check if iron curtain is already activated
    {
        insert_feedback("Iron Curtain Already Activated");
        return;
    }
    if (deposit < 10000) // Check if deposit is sufficient
    {
        insert_feedback("Deposit not enough(10000) to activate iron curtain");
        return;
    }
    insert_feedback("Iron Curtain Activated");
    deposit -= 10000;
    iron_curtain_counter = 30; // Reset the counter to 30 turns
}

/**
 * @brief Manages active protection system duration.
 * Decrements protection counter each turn and handles system deactivation when timer
 * expires. Generates system status notifications.
 */
void Game::check_iron_curtain(void)
{
    if (iron_curtain_counter >= 0) // Check if iron curtain is activated
    {
        iron_curtain_counter--;
        if (iron_curtain_counter <= 0) // Check if iron curtain time is up
        {
            insert_feedback("Iron Curtain Deactivated");
            iron_curtain_counter = -1; // Deactivate iron curtain
        }
    }
}

/**
 * @brief Operates automated missile interception system.
 * Generates defensive cruise missiles for each city under attack. Utilizes enhanced
 * missile parameters when corresponding upgrade research is completed.
 */
void Game::self_defense(void)
{
    if (!en_self_defense_sys) // Check if self-defense system is not researched
    {
        return;
    }
    for (auto &city : cities)
    {
        for (auto missile : missile_manager.get_attack_missiles())
        {
            if (missile_manager.create_cruise_missile(city, 100, en_enhanced_cruise_II ? 3 : 2)) // Try to create cruise missile
            {
                insert_feedback("Self Defense System Activated, Cruise Missile Launched");
            }
        }
    }
}
