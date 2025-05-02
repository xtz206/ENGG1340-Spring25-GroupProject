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
    std::vector<std::string> load_title(void);
    void reset(void);
};

class GeneralChecker
{
private:
    std::string folderpath = "./";

public:
    GeneralChecker() {};
    bool is_first_run(void);
    void save_first_run(void);
};

class SaveDumper
{
private:
    Game &game;
    std::string folderpath = "save/";

public:
    SaveDumper(Game &g) : game(g) {}
    bool is_slot_empty(const std::string& savename);
    bool save_game(const std::string& savename);
    void save_cities(const std::string& savepath);
    void save_general(const std::string& savepath);
    void save_attack_missiles(const std::string& savepath);
    void save_cruise_missiles(const std::string& savepath);
    void save_tech_tree(const std::string& savepath);
};

class SaveLoader
{
private:
    Game &game;
    std::string folderpath = "save/";

public:
    SaveLoader(Game &g) : game(g) {};
    bool is_slot_empty(const std::string& savename);
    bool load_game(const std::string &savename);
    void load_cities(const std::string &savepath);
    void load_general(const std::string &savepath);
    void load_attack_missiles(const std::string &savepath);
    void load_cruise_missiles(const std::string &savepath);
    void load_tech_tree(const std::string &savepath);
};
#endif
