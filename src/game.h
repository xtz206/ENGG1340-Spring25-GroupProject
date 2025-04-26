#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <algorithm>

#define inf 0x3f3f3f3f

class Position
{
    // NOTE: The Position is (y, x) instead of (x, y)
    //       This is to match the (line, col) system used by ncurses
public:
    union
    {
        int y;
        int l; // line
        int h; // height
    };
    union
    {
        int x;
        int c; // col
        int w; // width
    };

public:
    Position() : y(0), x(0) {}
    Position(int ny, int nx) : y(ny), x(nx) {}
    bool is_valid(void) const { return y < inf || x < inf; };
    bool operator==(const Position &p) const { return y == p.y && x == p.x; };
};
typedef Position Size;

class City
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;

private:
    Position position;
    std::string name;
    int hitpoint;
    int productivity;
    int countdown;
    int base_productivity;
    int cruise_storage;
    // TODO: add to config file or make it variable to difficulty
    const int cruise_build_time = 5;

public:
    City(Position p, std::string n, int hp);
    Position get_position(void) const { return position; };
    bool is_valid(void) const { return position.is_valid() && name != ""; };
};

enum class MissileType
{
    ATTACK,
    CRUISE,
    UNKNOWN
};

// TODO: add more detailed MissileProgress MissileDirection comments
enum class MissileProgress
{
    EXPLODED,   // Exploded
    HIT,        // Hit
    DESCENDING, // Descending
    ARRIVED,    // Arrived
    FLYING,     // Flying
    UNKNOWN     // Unknown
};

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

class Missile
{
    friend class Game;
    friend class GameRenderer;
    friend class MissileManager;

protected:
    int id;
    Position position;
    Position target;
    MissileProgress progress;
    MissileType type;
    int damage;
    int speed;

public:
    Missile(int i, Position p, Position t, int d, int v, MissileType tp);
    Position get_position(void) const { return position; };
    virtual Position get_target(void) = 0;
    MissileDirection get_direction(void);
    MissileProgress get_progress(void) const { return progress; };
    void move(void);
    virtual void move_step(void);
    void collide(void);
};

class AttackMissile : public Missile
{
    friend class Game;
    friend class GameRenderer;

private:
    City &city;

public:
    AttackMissile(int i, Position p, City &c, int d, int v);
    virtual Position get_target(void) override { return city.get_position(); };
    virtual void move_step(void) override;
};

class CruiseMissile : public Missile
{
    friend class Game;
    friend class GameRenderer;

private:
    Missile &missile;

public:
    CruiseMissile(int i, Position p, Missile &m, int d, int v);
    virtual Position get_target(void) override { return missile.get_position(); };
    virtual void move_step(void) override;
};

class MissileManager
{
    friend class Game;
    friend class GameRenderer;

private:
    int id;
    std::vector<City> &cities;
    std::vector<Missile *> missiles;
    std::vector<int> speed_list;
    std::vector<int> damage_list;
    std::vector<int> inc_turn; // NOTE: This controls how missile num increments by turn, a vector is left for diffrent level of difficulty
    // TODO: fix typo

    int generate_random(int turn, int hitpoint);
    int get_process_level(int turn, int hitpoint);

public:
    MissileManager(std::vector<City> &cts);
    std::vector<Missile *> get_missiles(void);
    std::vector<Missile *> get_attack_missiles(void);
    std::vector<Missile *> get_cruise_missiles(void);

    bool city_weight_check(City &c); // NOTE: this checks cities weight of becoming a target
    void create_attack_missile(Position p, City &c, int d, int v);
    bool create_cruise_missile(City &c, int d, int v);
    void update_missiles(void);
    void remove_missiles(void);

    void create_attack_wave(int turn, int hitpoint);
};

class TechNode
{
    friend class Game;
    friend class GameRenderer;
    friend class TechTree;
    friend class TechMenu;

private:
    std::string name;
    std::vector<std::string> description;
    int cost;
    int time;
    std::vector<TechNode *> prerequisites;

public:
    TechNode(const std::string &n, const std::vector<std::string> &d, int c, int t, const std::vector<TechNode *> p)
        : name(n), description(d), cost(c), time(t), prerequisites(p) {};
};
class TechTree
{
    friend class Game;
    friend class GameRenderer;
    friend class TechMenu;

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

class Game
{
    friend class GameRenderer;

private:
    bool activated;
    Size size;
    Position cursor;
    int turn;
    int deposit;
    int enemy_hitpoint;
    // NOTE: -1 means not built yet, 0 means built, otherwise means building (remaining time)
    int standard_bomb_counter = -1;
    int dirty_bomb_counter = -1;
    int hydrogen_bomb_counter = -1;
    std::vector<City> cities;
    std::vector<std::string> background;
    std::vector<std::string> feedbacks;
    MissileManager missile_manager;
    TechTree tech_tree;

    // TODO: clean up and reorganize the following comments
    bool en_enhanced_radar_I = false;
    bool en_enhanced_radar_II = false;
    bool en_enhanced_radar_III = false; // show attack missile details

    bool en_enhanced_cruise_I = false;   // done
    bool en_enhanced_cruise_II = false;  // done
    bool en_enhanced_cruise_III = false; // done

    bool en_fortress_city = false;      // done
    bool en_urgent_production = false;  // done
    bool en_evacuated_industry = false; // done

    bool en_dirty_bomb = false; // done
    bool en_fast_nuke = false;
    bool en_hydrogen_bomb = false; // done

    bool en_self_defense_sys = false;
    bool en_iron_curtain = false; // done
    bool iron_curtain_activated = false;
    int iron_curtain_cnt = 0;

public:
    Game(Size s, std::vector<City> cts, std::vector<std::string> bg);
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };

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
    void move_cursor(Position dcursor);
    void pass_turn(void);
    bool is_in_map(Position p) const { return p.y >= 0 && p.y < size.h && p.x >= 0 && p.x < size.w; };
    bool is_in_range(Position p1, Position p2, int range) const;
    bool is_game_over(void) const;
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
};

#endif
