#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <memory>

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
};
typedef Position Size;

class Loader;
class Missile;
class Economy;

class City
{
    friend class Game;
    friend class Renderer;
    friend class Economy;

private:
    Position position;
    std::string name;
    int hitpoint;
    int deposit;
    int productivity;

public:
    City(Position p, std::string n, int hp);
    Position get_position(void) const { return position; };
    bool is_in_range(Missile &missile) const;
};

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
    friend class Renderer;

protected:
    Position position;
    Position target;
    MissileProgress progress;
    int damage;
    int speed;

public:
    Missile(Position p, Position t, int d, int v);
    Position get_position(void) const { return position; };
    virtual Position get_target(void) = 0;
    MissileDirection get_direction(void);
    MissileProgress get_progress(void) const { return progress; };
    void move(void);
    virtual void move_step(void);
    void collide(void);
};

typedef std::shared_ptr<Missile> MissilePtr;

class AttackMissile : public Missile
{
    friend class Game;
    friend class Renderer;

private:
    City &city;

public:
    AttackMissile(Position p, City &c, int d, int v);
    virtual Position get_target(void) override { return city.get_position(); };
    virtual void move_step(void) override;
};

class CruiseMissile : public Missile
{
    friend class Game;
    friend class Renderer;

private:
    MissilePtr missile;

public:
    CruiseMissile(Position p, MissilePtr m, int d, int v);
    virtual Position get_target(void) override { return missile->get_position(); };
    virtual void move_step(void) override;
};

class Game
{
    friend class Renderer;
    friend class Economy;
private:
    Size size;
    Position cursor;
    int turn;
    std::vector<City> cities;
    std::vector<std::string> background;
    std::vector<MissilePtr> missiles;
    std::vector<MissilePtr> friendly_missiles;
    std::vector<MissilePtr> enemy_missiles;

    Economy *economy;

public:
    Game(Loader &ldr, Economy *eco);

    const Position &get_cursor(void) const { return cursor; };
    const std::vector<std::string> &get_background(void) const { return background; };
    int get_turn(void) const { return turn; };

    void move_cursor(Position dcursor);
    void pass_turn(void);
    bool is_in_map(Position p) const { return p.y >= 0 && p.y < size.h && p.x >= 0 && p.x < size.w; };
    City *select_city(void);
    City *select_city(Position p);

    void hit_city(City *city, int damage);
    void fix_city(void);
    void launch_cruise(void);
};

#endif