#ifndef SAVER_H
#define SAVER_H
#include <string>
#include <vector>
#include "game.h"

class Game;
class City;

class SaveDumper
{
    Game *game;
    std::string folderpath = "../save/";

public:
    SaveDumper(Game *game) : game(game) {}
    bool save_game(std::string index, bool if_cover = false);
    void save_game_general(std::string filepath);
    void save_attack_missile(std::string filepath);
    void save_cruise(std::string filepath);
    void save_city(std::string filepath);
    void save_tech_tree(std::string filepath);
};

class SaveLoader
{
    std::string dft_folderpath = "../save/";
    std::string folderpath;

public:
    SaveLoader() : folderpath(dft_folderpath){};
    std::vector<City> load_cities();
    void load_game_general(Game &game);
    void load_attack_missile(Game &game);
    void load_cruise(Game &game);
    void load_tech_tree(Game &game);
    bool load_game(Game &game, std::string index);
};
#endif