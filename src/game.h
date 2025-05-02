/**
 * @file game.h
 * @brief Header file for the game logic and core classes.
 *
 * This file contains the declarations of the main classes and enums used in the game,
 * including cities, missiles, technology tree, and the game itself. It defines the
 * relationships and interactions between these components.
 */

#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include "saver.h"
#include "utils.h"

#define inf 0x3f3f3f3f

/**
 * @class City
 * @brief Represents a city in the game with attributes such as position, name, hitpoints, productivity, and more.
 *
 * The City class encapsulates the properties and behaviors of a city in the game. It includes information
 * about the city's position, name, hitpoints, productivity, and other attributes. The class provides
 * functionality to retrieve the city's position and is designed to interact with other classes such as
 * Game, GameRenderer, MissileManager, SaveDumper, and SaveLoader.
 *
 * @private
 * @var Position position
 * The position of the city in the game world.
 *
 * @var std::string name The name of the city.
 *
 * @var int hitpoint The current hitpoints of the city.
 *
 * @var int productivity The current productivity of the city.
 *
 * @var int countdown A countdown timer associated with the city (purpose depends on game logic).
 *
 * @var int base_productivity The base productivity of the city.
 *
 * @var int cruise_storage The storage for cruise missiles in the city.
 *
 * @public
 * @fn City(Position p, const std::string &n, int hp)
 * Constructor to initialize a city with a position, name, and hitpoints.
 *
 * @fn Position get_position(void) const
 * Retrieves the position of the city.
 * @return The position of the city as a Position object.
 */
class City
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    Position position;
    std::string name;
    int hitpoint;
    int productivity;
    int countdown;
    int base_productivity;
    int cruise_storage;

public:
    City(Position p, const std::string &n, int hp);
    Position get_position(void) const { return position; };
};

/**
 * @enum MissileType
 * @brief Enum representing the type of missile.
 *
 * This enum defines the different types of missiles in the game, including attack and cruise missiles.
 */
enum class MissileType
{
    ATTACK,
    CRUISE,
    UNKNOWN
};

/**
 * @enum MissileDirection
 * @brief Enum representing the direction of a missile.
 *
 * This enum defines the possible directions a missile can take during its flight.
 */
enum class MissileDirection
{
    A,  // Arrived
    N,  // North
    NE, // North-East
    E,  // East
    SE, // South-East
    S,  // South
    SW, // South-West
    W,  // West
    NW, // North-West
    U   // Unknown
};

/**
 * @class Missile
 * @brief Represents a missile in the game with attributes such as position, target, type, and more.
 *
 * The Missile class encapsulates the properties and behaviors of a missile in the game. It includes information
 * about the missile's position, target, type, speed, damage, and explosion status. The class provides functionality
 * to move the missile and determine its direction.
 *
 * @private
 * @var int id Unique identifier for the missile.
 *
 * @var Position position The current position of the missile.
 *
 * @var Position target The target position of the missile.
 *
 * @var MissileType type The type of the missile (attack or cruise).
 *
 * @var bool is_exploded Indicates whether the missile has exploded.
 *
 * @var int damage The damage value of the missile.
 *
 * @var int speed The speed of the missile.
 */
class Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;
    friend class SaveLoader;

protected:
    int id;
    Position position;
    Position target;
    MissileType type;
    bool is_exploded;
    int damage;
    int speed;

public:
    Missile(int i, Position p, Position t, int d, int v, MissileType tp);
    Position get_position(void) const { return position; };
    virtual Position get_target(void) = 0;
    MissileType get_type(void) const { return type; };
    MissileDirection get_direction(void);
    bool get_is_exploded(void) const { return is_exploded; };
    void set_is_exploded(void) { is_exploded = true; };
    void move(void);
    virtual void move_step(void);
};

/**
 * @class AttackMissile
 * @brief Represents an attack missile targeting a specific city in the game.
 *
 * The AttackMissile class inherits from the Missile class and is responsible
 * for targeting and moving towards a specific city. It interacts with other
 * game components such as Game, GameRenderer, MissileManager, SaveDumper,
 * and SaveLoader through friendship relationships.
 */
class AttackMissile : public Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    City &city;
    bool is_aimed = false;

public:
    AttackMissile(int i, Position p, City &c, int d, int v);
    virtual Position get_target(void) override { return city.get_position(); };
    virtual void move_step(void) override;
};

/**
 * @class CruiseMissile
 * @brief Represents a specialized type of missile that targets a specific entity and moves accordingly.
 *
 * The CruiseMissile class extends the functionality of the Missile class by introducing a target ID
 * and overriding movement behavior. It is designed to interact with other components such as the
 * Game, GameRenderer, MissileManager, and SaveDumper through friend relationships.
 */
class CruiseMissile : public Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;

private:
    Missile &missile;
    int target_id;

public:
    CruiseMissile(int i, Position p, Missile &m, int d, int v, int t_id);
    virtual Position get_target(void) override { return missile.get_position(); };
    virtual void move_step(void) override;
};

/**
 * @class MissileManager
 * @brief Manages the creation, updating, and removal of missiles in the game.
 *
 * The MissileManager class is responsible for handling all missile-related operations,
 * including generating attack and cruise missiles, updating their states, and managing
 * their interactions with cities. It also supports difficulty settings and wave generation.
 */
class MissileManager
{
    friend class Game;
    friend class GameRenderer;
    friend class SaveDumper;
    friend class SaveLoader;
    friend class AssetLoader;

private:
    int id;
    Size size;
    std::vector<City> &cities;
    std::vector<Missile *> missiles;
    std::array<int, 5> speed_list = {0};
    std::array<int, 5> damage_list = {0};
    // NOTE: controls how missile num in a wave increases by turn
    std::array<int, 3> inc_turn = {50, 30, 20};
    int generate_random(int turn, int hitpoint);
    int get_process_level(int turn, int hitpoint);

public:
    MissileManager(std::vector<City> &cts);
    std::vector<Missile *> get_missiles(void);
    std::vector<Missile *> get_attack_missiles(void);
    std::vector<Missile *> get_cruise_missiles(void);

    void set_difficulty(int lv);
    bool city_weight_check(City &c);
    void create_attack_missile(Position p, City &c, int d, int v);
    bool create_cruise_missile(City &c, int d, int v);
    void update_missiles(void);
    void remove_missiles(void);

    void create_attack_wave(int turn, int difficulty_level, int hitpoint);
};

/**
 * @class TechNode
 * @brief Represents a node in a technology tree, encapsulating information about a specific technology.
 *
 * The TechNode class is used to define individual technologies in a tech tree. Each node contains
 * details such as its name, description, cost, time to research, and prerequisites.
 *
 */
class TechNode
{
    friend class Game;
    friend class GameRenderer;
    friend class TechTree;
    friend class TechMenu;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    std::string name;
    std::vector<std::string> description;
    int cost;
    int time;
    std::vector<TechNode *> prerequisites;

public:
    /**
     * @brief Constructor for the TechNode class.
     *
     * @param n Name of the technology.
     * @param d Description of the technology.
     * @param c Cost of the technology.
     * @param t Time required to research the technology.
     * @param p Prerequisites for the technology.
     */
    TechNode(const std::string &n, const std::vector<std::string> &d, int c, int t, const std::vector<TechNode *> p)
        : name(n), description(d), cost(c), time(t), prerequisites(p) {};
};

/**
 * @class TechTree
 * @brief Represents a technology tree in the game, managing the research and availability of technologies.
 *
 * The TechTree class is responsible for handling the research process of technologies,
 * tracking which technologies have been researched, and determining which technologies are available.
 * It interacts with other classes such as Game, GameRenderer, TechMenu, SaveDumper, and SaveLoader.
 */
class TechTree
{
    friend class Game;
    friend class GameRenderer;
    friend class TechMenu;
    friend class SaveDumper;
    friend class SaveLoader;
    friend class AssetLoader;

private:
    std::vector<TechNode *> nodes;
    std::vector<TechNode *> researched;
    std::vector<TechNode *> available;
    TechNode *researching;
    TechNode *prev_researching;
    int remaining_time;

public:
    TechTree(void);
    ~TechTree(void);

    std::vector<std::string> get_tech_names(void) const;

    void start_research(TechNode *node);
    void proceed_research(void);
    bool check_research(void);
    bool is_researched(TechNode *node) const { return std::find(researched.begin(), researched.end(), node) != researched.end(); };
    bool is_available(TechNode *node) const { return std::find(available.begin(), available.end(), node) != available.end(); };
    void update_available(int deposit);

private:
    bool check_available(TechNode *node, int deposit) const;
};

/**
 * @class Game
 * @brief Represents the main game logic, managing the state of the game, cities, missiles, and technology.
 *
 * The Game class encapsulates the core functionality of the game, including managing cities,
 * missiles, technology research, and game state. It provides methods for interacting with
 * the game world and updating its state based on player actions and game events.
 */
class Game
{
    friend class GameRenderer;
    friend class SaveDumper;
    friend class SaveLoader;
    friend class AssetLoader;
    friend class OperationMenu;

private:
    Size size;
    Position cursor;
    int turn;
    int deposit;
    int difficulty_level;
    int enemy_hitpoint;
    int score;
    int casualty;

    std::vector<City> cities;
    std::vector<std::string> background;
    VAttrString feedbacks;
    MissileManager missile_manager;
    TechTree tech_tree;

    // NOTE: super weapon flags
    //       -1 means not built yet, 0 means built,
    //       otherwise means building / remaining time
    int standard_bomb_counter = -1;
    int dirty_bomb_counter = -1;
    int hydrogen_bomb_counter = -1;
    int iron_curtain_counter = -1;

    // NOTE: technology researched flags
    bool en_enhanced_radar_I = false;
    bool en_enhanced_radar_II = false;
    bool en_enhanced_radar_III = false;
    bool en_enhanced_cruise_I = false;
    bool en_enhanced_cruise_II = false;
    bool en_enhanced_cruise_III = false;
    bool en_fortress_city = false;
    bool en_urgent_production = false;
    bool en_evacuated_industry = false;
    bool en_dirty_bomb = false;
    bool en_fast_nuke = false;
    bool en_hydrogen_bomb = false;
    bool en_self_defense_sys = false;
    bool en_iron_curtain = false;

public:
    Game(void) : missile_manager(cities) {};
    void set_difficulty(int lv);

    const Size &get_size(void) const { return size; };
    const Position &get_cursor(void) const { return cursor; };
    const std::vector<std::string> &get_background(void) const { return background; };
    const VAttrString &get_feedbacks(void) const { return feedbacks; };
    MissileManager &get_missile_manager(void) { return missile_manager; };
    TechTree &get_tech_tree(void) { return tech_tree; };
    int get_turn(void) const { return turn; };
    std::vector<Missile *> get_missiles(void) { return missile_manager.get_missiles(); };
    void insert_feedback(const AttrString &feedback);
    void insert_feedback(const std::string &feedback, attr_t attr) { insert_feedback(AttrString(feedback, attr)); };
    int get_deposit(void) const { return deposit; };
    int get_productivity(void) const;
    int get_enemy_hp(void) const { return enemy_hitpoint; };

    // NOTE: cursor/position-related functions
    void move_cursor(Position dcursor);
    void move_cursor_to_city(int index);
    void pass_turn(void);
    bool is_in_map(Position p) const { return p.y >= 0 && p.y < size.h && p.x >= 0 && p.x < size.w; };
    bool is_in_range(Position p1, Position p2, int range) const;
    bool is_on_sea(Position p) const { return background.at(p.y).at(p.x) == '#'; };
    bool is_on_city(Position p) const { return background.at(p.y).at(p.x) == '@'; };
    bool is_on_land(Position p) const { return background.at(p.y).at(p.x) == ' '; };
    bool is_selected_missile(void);
    bool is_selected_city(void);
    Missile &select_missile(void);
    City &select_city(void);

    // NOTE: production/research/fix-related functions
    void start_research(TechNode *node);
    void check_research(void);
    void finish_research(TechNode *node);
    void hit_city(City &city, int damage);
    void fix_city(void);
    void build_cruise(void);
    void launch_cruise(void);
    void build_standard_bomb(void);
    void launch_standard_bomb(void);
    void build_dirty_bomb(void);
    void launch_dirty_bomb(void);
    void build_hydrogen_bomb(void);
    void launch_hydrogen_bomb(void);
    void activate_iron_curtain(void);
    void check_iron_curtain(void);
    void self_defense(void);

    // NOTE: score-related functions
    int get_score(void) const { return score; };
    int get_casualty(void) const { return casualty; };
    bool check_game_over(void);
};

#endif
