/**
 * @file game.h
 * @brief Defines core game components and logic for Missile Commander
 */
#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <algorithm>
#include "saver.h"
#include <deque>

#define inf 0x3f3f3f3f ///< Special value representing infinity

// ===================== POSITION & SIZE ===================== //

/**
 * @class Position
 * @brief Represents 2D coordinates in game world
 * 
 * Uses (y, x) coordinate system to match ncurses (line, column) conventions.
 * Provides union aliases for different context usages:
 * - Map coordinates: y/x
 * - Screen position: l(line)/c(column)
 * - Dimension representation: h(height)/w(width)
 */
class Position
{
    // NOTE: The Position is (y, x) instead of (x, y)
    //       This is to match the (line, col) system used by ncurses
public:
    union
    {
        int y; ///< Vertical coordinate (map system)
        int l; // line
        int h; // height
    };
    union
    {
        int x; ///< Horizontal coordinate (map system)
        int c; // col
        int w; // width
    };

public:
    Position() : y(0), x(0) {}
    Position(int ny, int nx) : y(ny), x(nx) {}

    /// @name Position Validation
    /// @{
    bool is_valid(void) const { return y < inf || x < inf; }; ///< Check if position contains valid values
    bool operator==(const Position &p) const { return y == p.y && x == p.x; }; ///< Equality comparison
    /// @}
};
typedef Position Size; ///< Type alias for dimension representation

// ===================== CITY SYSTEM ===================== //

/**
 * @class City
 * @brief Represents a defendable city in the game
 * 
 * Contains city status information including:
 * - Position on game map
 * - Defensive capabilities
 * - Production capacity
 * - Missile storage
 */

class City
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    Position position; ///< Map coordinates
    std::string name; ///< City identifier
    int hitpoint; ///< Current health points
    int productivity; ///< Current production capacity
    int countdown; //< Production timer
    int base_productivity; ///< Base production value
    int cruise_storage; ///< Stored cruise missiles

public:
    City(Position p, const std::string &n, int hp);

    /// @name Accessors
    /// @{
    Position get_position(void) const { return position; }; ///< Get map position
    bool is_valid(void) const { return position.is_valid() && name != ""; }; ///< Validate city state
    /// @}
};

// ===================== MISSILE SYSTEM ===================== //

/**
 * @enum MissileType
 * @brief Categorizes missile behaviors
 */
enum class MissileType
{
    ATTACK, ///< Enemy missiles targeting cities
    CRUISE, ///< Player-controlled defensive missiles
    UNKNOWN  ///< Default/error state
};

/**
 * @enum MissileDirection
 * @brief Represents movement directions using compass rose convention
 */
enum class MissileDirection
{
    A,  // Arrived ///< Arrived at target
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
 * @brief Base class for all missile types
 * 
 * Implements core missile behavior including:
 * - Movement calculations
 * - Damage handling
 * - Target tracking
 */
class Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;
    friend class SaveLoader;

protected:
    int id; ///< Unique identifier
    Position position; ///< Current coordinates
    Position target;  ///< Destination coordinates
    MissileType type; ///< Behavior category
    bool is_exploded; ///< Detonation status
    int damage; ///< Impact damage value
    int speed; ///< Movement units per turn

public:
    Missile(int i, Position p, Position t, int d, int v, MissileType tp);

     /// @name Core Functionality
    /// @{
    Position get_position(void) const { return position; };
    virtual Position get_target(void) = 0;  ///< Pure virtual target acquisition
    MissileDirection get_direction(void); ///< Calculate current heading
    bool get_is_exploded(void) const { return is_exploded; };
    void set_is_exploded(void) { is_exploded = true; };
    void move(void); ///< Execute movement logic
    virtual void move_step(void); ///< Virtual movement implementation
    /// @}
};

/**
 * @class AttackMissile
 * @brief Offensive missiles targeting cities
 * 
 * Implements specific behavior for enemy attack missiles:
 * - City targeting logic
 * - Aimed trajectory calculations
 */
class AttackMissile : public Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    City &city; ///< Target city reference
    bool is_aimed = false; ///< Target lock status

public:
    AttackMissile(int i, Position p, City &c, int d, int v);
    /// @name Overrides
    /// @{
    virtual Position get_target(void) override { return city.get_position(); };
    virtual void move_step(void) override; ///< Implement aimed trajectory
    /// @}
};

/**
 * @class CruiseMissile
 * @brief Defensive intercept missiles
 * 
 * Implements counter-offensive behavior:
 * - Dynamic target tracking
 * - Interception course calculations
 */
class CruiseMissile : public Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;
    friend class SaveDumper;

private:
    Missile &missile; ///< Target missile reference
    int target_id; ///< Tracked missile ID

public:
    CruiseMissile(int i, Position p, Missile &m, int d, int v, int t_id);

    /// @name Overrides
    /// @{
    virtual Position get_target(void) override { return missile.get_position(); };
    virtual void move_step(void) override; ///< Implement pursuit logic
    /// @}
};

// ===================== MISSILE MANAGEMENT ===================== //

/**
 * @class MissileManager
 * @brief Central controller for missile operations
 * 
 * Manages:
 * - Missile wave generation
 * - Lifetime tracking
 * - Collision detection
 * - Performance optimization
 */
class MissileManager
{
    friend class Game;
    friend class GameRenderer;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    int id; ///< Missile ID counter
    std::vector<City> &cities; ///< City list reference
    std::vector<Missile *> missiles;  ///< Active missile pointers
    std::vector<int> speed_list; ///< Speed configuration
    std::vector<int> damage_list; ///< Damage configuration
    // NOTE: controls how missile num in a wave increases by turn
    std::vector<int> inc_turn; ///< Wave difficulty progression

    /// @name Internal Logic
    /// @{
    int generate_random(int turn, int hitpoint); ///< Procedural wave generation
    int get_process_level(int turn, int hitpoint); ///< Difficulty scaling
    /// @}

public:
    MissileManager(std::vector<City> &cts);
    /// @name Missile Access
    /// @{
    std::vector<Missile *> get_missiles(void); ///< All active missiles
    std::vector<Missile *> get_attack_missiles(void); ///< Offensive missiles
    std::vector<Missile *> get_cruise_missiles(void); ///< Defensive missiles
    /// @}
    
    /// @name Operations
    /// @{

    bool city_weight_check(City &c); // NOTE: this checks cities weight of becoming a target ///< Target priority calculation
    void create_attack_missile(Position p, City &c, int d, int v); ///< Spawn enemy missile
    bool create_cruise_missile(City &c, int d, int v); ///< Launch defense
    void update_missiles(void); ///< Frame update handler
    void remove_missiles(void); ///< Cleanup expired missiles

    void create_attack_wave(int turn, int difficulty_level, int hitpoint); ///< Generate enemy wave

    void reset(void); ///< Clear all missiles
    /// @}
};

// ===================== TECHNOLOGY TREE ===================== //

/**
 * @class TechNode
 * @brief Represents a researchable technology
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
    std::string name; ///< Technology name
    std::vector<std::string> description; ///< Detailed description
    int cost; ///< Research cost
    int time;  ///< Research duration
    std::vector<TechNode *> prerequisites; ///< Required technologies

public:
    TechNode(const std::string &n, const std::vector<std::string> &d, int c, int t, const std::vector<TechNode *> p)
        : name(n), description(d), cost(c), time(t), prerequisites(p) {};
};

/**
 * @class TechTree
 * @brief Manages technology research system
 * 
 * Handles:
 * - Research progression
 * - Dependency resolution
 * - Availability updates
 */
class TechTree
{
    friend class Game;
    friend class GameRenderer;
    friend class TechMenu;
    friend class SaveDumper;
    friend class SaveLoader;

private:
    std::vector<TechNode *> nodes; ///< All available technologies
    std::vector<TechNode *> researched; ///< Completed research
    std::vector<TechNode *> available; ///< Currently researchable
    TechNode *researching; ///< Currently researching
    TechNode *prev_researching; ///< Previously researched
    int remaining_time; ///< Research progress

public:
    TechTree(void);
    ~TechTree(void);

    /// @name Research Operations
    /// @{
    std::vector<std::string> get_tech_names(void) const;

    void start_research(TechNode *node); ///< Begin research
    void proceed_research(void); ///< Advance research
    bool check_research(void); ///< Complete research
    /// @}

    /// @name Status Checks
    /// @{
    bool is_researched(TechNode *node) const { return std::find(researched.begin(), researched.end(), node) != researched.end(); };  ///< Check completion status
    bool is_available(TechNode *node) const { return std::find(available.begin(), available.end(), node) != available.end(); }; ///< Check researchability
    /// @}

    /// @name System Management
    /// @{
    void update_available(int deposit); ///< Refresh available techs
    void reset(void);  ///< Clear research state
    /// @}
private:
    bool check_available(TechNode *node, int deposit) const; ///< Validate prerequisites
};

// ===================== GAME CORE ===================== //

/**
 * @struct CasualtyReport
 * @brief Tracks damage events for post-game analysis
 */
struct CasualtyReport {
    std::string city_name; ///< Affected city
    int damage; ///< Damage sustained
    int remaining_hp; ///< Post-damage health
    int turn; ///< Turn number
};

/**
 * @class Game
 * @brief Central game state controller
 * 
 * Manages complete game lifecycle including:
 * - World state
 * - Player interactions
 * - Victory conditions
 * - Resource management
 */
class Game
{
    friend class GameRenderer;
    friend class SaveDumper;
    friend class SaveLoader;
    friend class OperationMenu;

private:
    // Core State
    bool activated;  ///< Game active flag
    Size size; ///< Map dimensions
    Position cursor; ///< Player cursor
    int turn; ///< Current turn
    int deposit; ///< Resource points
    int difficulty_level;  ///< Difficulty setting
    int enemy_hitpoint; ///< Enemy collective HP
    // NOTE: -1 means not built yet, 0 means built, otherwise means building (remaining time)
    int standard_bomb_counter = -1; ///< Standard nuke status
    int dirty_bomb_counter = -1; ///< Dirty bomb status
    int hydrogen_bomb_counter = -1; ///< Hydrogen bomb status
    std::vector<City> cities; ///< City list
    std::vector<std::string> background;
    std::vector<std::string> feedbacks;
    std::deque<CasualtyReport> casualty_reports; ///< Damage history
    MissileManager missile_manager;  ///< Missile controller
    // Technology System
    TechTree tech_tree; //< Research manager

    // NOTE: technology researched flags
    bool en_enhanced_radar_I = false; ///< Radar upgrade I
    bool en_enhanced_radar_II = false; ///< Radar upgrade II
    bool en_enhanced_radar_III = false; ///< Radar upgrade III
    bool en_enhanced_cruise_I = false; ///< Cruise missile upgrade I
    bool en_enhanced_cruise_II = false; ///< Cruise missile upgrade II
    bool en_enhanced_cruise_III = false;///< Cruise missile upgrade III
    bool en_fortress_city = false; ///< City fortification
    bool en_urgent_production = false; ///< Emergency production
    bool en_evacuated_industry = false; ///< Industry protection
    bool en_dirty_bomb = false; ///< Dirty bomb unlocked
    bool en_fast_nuke = false; ///< Rapid deployment
    bool en_hydrogen_bomb = false; ///< H-bomb unlocked
    bool en_self_defense_sys = false; ///< Auto-defense
    bool en_iron_curtain = false; ///< Force field

    // Iron Curtain
    // NOTE: iron curtain activation and countdown
    bool iron_curtain_activated = false; //< Force field active
    int iron_curtain_cnt = 0; ///< Force field duration

    int score = 0; ///< Player score

public:
    

    Game(Size s, std::vector<City> cts, std::vector<std::string> bg);
    /// @name Configuration
    /// @{
    // NOTE: set difficulty and params used by missile_manager
    void set_difficulty(int lv); ///< Set difficulty parameters
    /// @}

    /// @name State Management
    /// @{
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };
    /// @}
    
    /// @name Game Operations
    /// @{

    const Position &get_cursor(void) const { return cursor; }; 
    const std::vector<std::string> &get_background(void) const { return background; }; 
    const std::vector<std::string> &get_feedbacks(void) const { return feedbacks; };  
    MissileManager &get_missile_manager(void) { return missile_manager; };
    TechTree &get_tech_tree(void) { return tech_tree; };
    int get_turn(void) const { return turn; };
    std::vector<Missile *> get_missiles(void) { return missile_manager.get_missiles(); };
    std::vector<std::string> get_general_info(void);
    std::vector<std::string> get_selected_info(void);
    std::vector<std::string> get_tech_info(void) const;
    std::vector<std::string> get_super_weapon_info(void) const;
    std::vector<std::string> get_feedback_info(void) const;
    void insert_feedback(const std::string &feedback);
    int get_deposit(void) const { return deposit; };
    int get_productivity(void) const;
    int get_enemy_hp(void) const { return enemy_hitpoint; };

    // NOTE: cursor/position-related functions
    void move_cursor(Position dcursor); ///< Cursor movement
    void move_cursor_to_city(int index); ///< City quick-select
    void pass_turn(void);  ///< Advance game state
    bool is_in_map(Position p) const { return p.y >= 0 && p.y < size.h && p.x >= 0 && p.x < size.w; };
    bool is_in_range(Position p1, Position p2, int range) const;
    bool is_game_over(void) const; ///< Victory check
    /// @}
    
    /// @name Combat Operations
    /// @{
    bool is_selected_missile(void);
    bool is_selected_city(void);
    Missile &select_missile(void);
    City &select_city(void);

    // NOTE: production/research/fix-related functions
    void start_research(TechNode *node);
    void check_research(void);
    void finish_research(TechNode *node);
    void hit_city(City &city, int damage); ///< Apply city damage
    void fix_city(void); ///< Repair city
    void build_cruise(void); ///< Produce defense
    void launch_cruise(void); ///< Deploy defense
    void build_standard_bomb(void); ///< Nuke production
    void launch_standard_bomb(void); ///< Nuke deployment
    void build_dirty_bomb(void);
    void launch_dirty_bomb(void);
    void build_hydrogen_bomb(void);
    void launch_hydrogen_bomb(void);
    void activate_iron_curtain(void); ///< Enable force field
    /// @}
    void check_iron_curtain(void);
    void self_defense(void);

    void reset(void); ///< Reset game state

    /// @name Scoring
    /// @{
    int get_score() const { return score; }
    void add_score(int value) { score += value; }
    /// @}
    
    /// @name Casualty Tracking
    /// @{
    void add_casualty_report(const std::string &name, int dmg, int hp) {
        casualty_reports.push_front({name, dmg, hp, turn});
        if (casualty_reports.size() > 5) casualty_reports.pop_back(); 
    }
    const std::deque<CasualtyReport>& get_casualty_reports() const { return casualty_reports; }
    /// @}
};

#endif
