#ifndef SAVER_H
#define SAVER_H
#include <string>
#include <vector>
#include "game.h"

class Game;
class City;

class Saver
{
    Game* game;
    std::string folderpath = "../save/";
public:
    Saver(Game* game) : game(game){}
    std::string generate_folder_name_by_time();
    void save_game();
    void save_game_general(std::string filepath);
    void save_attack_missile(std::string filepath);
    void save_cruise(std::string filepath);
    void save_city(std::string filepath);
    void save_tech_tree(std::string filepath);
};

class LogLoader
{
    std::string dft_folderpath = "../save/";
    std::string folderpath;
public:
    LogLoader(std::string fpath) : folderpath(dft_folderpath+fpath) {};
    std::vector<City> load_cities();
    void load_game_general(Game &game);
    void load_attack_missile(Game &game);
    void load_cruise(Game &game);
    void load_tech_tree(Game &game);
    void load_game(Game &game);
};
#endif