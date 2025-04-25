#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

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
};
typedef Position Size;

class City
{
    friend class Game;
    friend class GameRenderer;

private:
    Position position;
    std::string name;
    int hitpoint;
    int deposit;
    int productivity;
    int countdown;

public:
    static const City null_city;
    City() : position(inf, inf), name(""), hitpoint(0), deposit(0), productivity(0), countdown(0) {};
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
    std::vector<int> inc_turn; // This controls how missile num increments by turn, a vector is left for diffrent level of difficulty
    int hitpoint;

public:
    MissileManager(std::vector<City> &cts);
    std::vector<Missile *> get_missiles(void);
    std::vector<Missile *> get_attack_missiles(void);
    std::vector<Missile *> get_cruise_missiles(void);
    void create_attack_missile(Position p, City &c, int d, int v);
    bool create_cruise_missile(City &c, int d, int v);
    void update_missiles(void);
    void remove_missiles(void);

    int generate_random(int turn);
    int get_process_level(int turn);
    void create_attack_wave(int turn);
};

class Game
{
    friend class GameRenderer;

private:
    bool activated;
    Size size;
    Position cursor;
    int turn;
    std::vector<City> cities;
    std::vector<std::string> background;
    MissileManager missile_manager;

public:
    Game(Size s, std::vector<City> cts, std::vector<std::string> bg);
    bool is_activated(void) { return activated; };
    void activate(void) { activated = true; };
    void deactivate(void) { activated = false; };

    const Position &get_cursor(void) const { return cursor; };
    const std::vector<std::string> &get_background(void) const { return background; };
    int get_turn(void) const { return turn; };
    std::vector<Missile *> get_missiles(void) { return missile_manager.get_missiles(); };

    int get_total_deposit(void) const;
    int get_total_productivity(void) const;

    void move_cursor(Position dcursor);
    void pass_turn(void);
    bool is_in_map(Position p) const { return p.y >= 0 && p.y < size.h && p.x >= 0 && p.x < size.w; };
    bool is_in_range(Position p1, Position p2, int range) const;
    bool is_game_over(void) const;
    City &select_city(void);

    void hit_city(City &city, int damage);
    void fix_city(void);
    void launch_cruise(void);
};

#endif
