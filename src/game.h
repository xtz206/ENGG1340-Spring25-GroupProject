#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <memory>

typedef std::pair<int, int> Coordinate; // NOTE: Coordinate is (y, x) / (line, col) in this game
class Loader;

class City
{
    friend class Game;
    friend class Renderer;

private:
    Coordinate position;
    std::string name;
    int hitpoint;

public:
    City(Coordinate c, std::string n, int hp);
    Coordinate get_position(void) const { return position; };
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
    Coordinate position;
    Coordinate target;
    MissileProgress progress;
    int damage;
    int speed;

public:
    Missile(Coordinate p, Coordinate t, int d, int v);
    Coordinate get_position(void) const { return position; };
    virtual Coordinate get_target(void) = 0;
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
    AttackMissile(Coordinate p, City &c, int d, int v);
    virtual Coordinate get_target(void) override { return city.get_position(); };
    virtual void move_step(void) override;
};

class CruiseMissile : public Missile
{
    friend class Game;
    friend class Renderer;

private:
    MissilePtr missile;

public:
    CruiseMissile(Coordinate p, MissilePtr m, int d, int v);
    virtual Coordinate get_target(void) override { return missile->get_position(); };
    virtual void move_step(void) override;
};

class Game
{
    friend class Renderer;

private:
    Coordinate size;
    Coordinate cursor;
    int turn;
    std::vector<City> cities;
    std::vector<std::string> background;
    std::vector<MissilePtr> missiles;
    std::vector<MissilePtr> friendly_missiles;
    std::vector<MissilePtr> enemy_missiles;

public:
    Game(Loader &ldr);

    const Coordinate &get_cursor(void) const { return cursor; };
    const std::vector<std::string> &get_background(void) const { return background; };
    int get_turn(void) const { return turn; };

    void move_cursor(int dline, int dcol);
    void pass_turn(void);
    bool is_in_map(Coordinate c) const { return c.first >= 0 && c.first < size.first && c.second >= 0 && c.second < size.second; };
    City *select_city(void);
    City *select_city(Coordinate c);

    void hit_city(City *city, int damage);
    void fix_city(void);
    void launch_cruise(void);
};

#endif