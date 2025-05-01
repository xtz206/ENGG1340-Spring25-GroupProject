#ifndef SAVER_H
#define SAVER_H
#include <string>
#include <vector>
#include "game.h"

class Game;
class City;

class AssetLoader
{
private:
    Game &game;

public:
    AssetLoader(Game &g) : game(g) {};
    void load_general(void);
    void load_background(void);
    void load_cities(void);
    void reset(void);
};

class SaveDumper
{
private:
    Game &game;
    std::string folderpath = "../save/";

public:
    SaveDumper(Game &g) : game(g) {}
    bool is_slot_empty(std::string index);
    bool save_game(std::string index);
    void save_cities(std::string filepath);
    void save_general(std::string filepath);
    void save_attack_missiles(std::string filepath);
    void save_cruise_missiles(std::string filepath);
    void save_tech_tree(std::string filepath);
};

class SaveLoader
{
private:
    Game &game;
    std::string folderpath;

public:
    SaveLoader(Game &g) : game(g), folderpath("../save/") {};
    bool is_slot_empty(std::string index);
    bool load_game(const std::string &index);
    void load_cities();
    void load_general();
    void load_attack_missiles();
    void load_cruise_missiles();
    void load_tech_tree();
};
#endif