#ifndef SAVER_H
#define SAVER_H
#include <string>
#include <vector>
#include "game.h"

class Game;
class City;

class SaveDumper
{
    Game &game;
    std::string folderpath = "../save/";

public:
    SaveDumper(Game &g) : game(g) {}
    bool save_game(std::string index, bool if_cover = false);
    void save_game_general(std::string filepath);
    void save_attack_missile(std::string filepath);
    void save_cruise(std::string filepath);
    void save_city(std::string filepath);
    void save_tech_tree(std::string filepath);
};

class SaveLoader
{
    Game &game;
    // TODO: rename variable
    std::string dft_folderpath = "../save/";
    std::string folderpath;

public:
    SaveLoader(Game &g) : game(g), folderpath(dft_folderpath) {};
    void load_cities();
    void load_general();
    void load_attack_missile();
    void load_cruise();
    void load_tech_tree();
    bool load_game(const std::string &index);
};
#endif