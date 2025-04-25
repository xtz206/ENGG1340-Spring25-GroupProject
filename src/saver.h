#include <string>
#include <vector>
#include <game.h>

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