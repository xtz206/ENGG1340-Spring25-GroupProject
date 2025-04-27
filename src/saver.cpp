#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "saver.h"
#include "game.h"

void SaveDumper::save_game_general(std::string filepath)
{
    std::string filename = filepath + "general.txt";
    std::ofstream general_log(filename);
    if (general_log.is_open())
    {
        general_log << "level:" << game->difficulty_level << "\n";
        general_log << "turn:" << game->get_turn() << "\n";
        general_log << "deposit:" << game->get_deposit() << "\n";
        general_log << "cursor_y:" << game->cursor.y << "\n";
        general_log << "cursor_x:" << game->cursor.x << "\n";
        general_log << "standard_bomb_counter:" << game->standard_bomb_counter << "\n";
        general_log << "dirty_bomb_counter:" << game->dirty_bomb_counter<< "\n";
        general_log << "hydrogen_bomb_counter:" << game->hydrogen_bomb_counter<< "\n";
        general_log << "enemy_hitpoint:" << game->enemy_hitpoint << "\n";
        general_log << "iron_curtain_activated:" << game->iron_curtain_activated << "\n";
        general_log << "enhanced_radar_I:" << game->en_enhanced_radar_I << "\n";
        general_log << "enhanced_radar_II:" << game->en_enhanced_radar_II << "\n";
        general_log << "enhanced_radar_III:" << game->en_enhanced_radar_III << "\n";
        general_log << "enhanced_cruise_I:" << game->en_enhanced_cruise_I << "\n";
        general_log << "enhanced_cruise_II:" << game->en_enhanced_cruise_II << "\n";
        general_log << "enhanced_cruise_III:" << game->en_enhanced_cruise_III << "\n";
        general_log << "fortress_city:" << game->en_fortress_city << "\n";
        general_log << "urgent_production:" << game->en_urgent_production << "\n";
        general_log << "evacuated_industry:" << game->en_evacuated_industry << "\n";
        general_log << "dirty_bomb:" << game->en_dirty_bomb << "\n";
        general_log << "fast_nuke:" << game->en_fast_nuke << "\n";
        general_log << "hydrogen_bomb:" << game->en_hydrogen_bomb << "\n";
        general_log << "self_defense_sys:" << game->en_self_defense_sys << "\n";
        general_log << "iron_curtain:" << game->en_iron_curtain << "\n";
        general_log << "missile_manager_id" << game->missile_manager.id << "\n";
    }
    general_log.close();
}

void SaveDumper::save_attack_missile(std::string filepath)
{
    std::string filename = filepath + "atkmissiles.txt";
    std::ofstream atkmissile_log(filename);
    if (atkmissile_log.is_open())
    {
        atkmissile_log << "id,y,x,target_y,target_x,damage,speed,is_aimed\n";
        for (auto &missile : game->missile_manager.get_attack_missiles())
        {
            AttackMissile *attack_missile = dynamic_cast<AttackMissile *>(missile);
            atkmissile_log << missile->id << "," << missile->get_position().y << "," << missile->get_position().x << "," << missile->target.y << "," << missile->target.x << "," << missile->damage << "," << missile->speed << "," << attack_missile->is_aimed << "\n";
        }
    }
    atkmissile_log.close();
}

void SaveDumper::save_cruise(std::string filepath)
{
    std::string filename = filepath + "cruise.txt";
    std::ofstream cruise_log(filename);
    if (cruise_log.is_open())
    {
        cruise_log << "id,y,x,target_id,damage,speed\n";

        for (auto &missile : game->missile_manager.get_cruise_missiles())
        {
            CruiseMissile *cruise_missile = dynamic_cast<CruiseMissile *>(missile);
            cruise_log << missile->id << "," << missile->get_position().y << "," << missile->get_position().x << "," << cruise_missile->target_id << "," << missile->damage << "," << missile->speed << "\n";
        }
    }
    cruise_log.close();
}

void SaveDumper::save_city(std::string filepath)
{
    std::string filename = filepath + "cities.txt";
    std::ofstream city_log(filename);
    if (city_log.is_open())
    {
        city_log << "Name,y,x,hitpoint,base_productivity,productivity,cruise_storage,countdown\n";
        for (const auto &city : game->cities)
        {
            city_log << city.name << "," << city.position.y << "," << city.position.x << ","
                     << city.hitpoint << "," << city.base_productivity << ","
                     << city.productivity << "," << city.cruise_storage << "," << city.countdown << "\n";
        }
    }
    city_log.close();
}

void SaveDumper::save_tech_tree(std::string filepath)
{
    std::string filename = filepath + "tech_tree.txt";
    std::ofstream tech_tree_log(filename);
    if (tech_tree_log.is_open())
    {
        tech_tree_log << "researched,";
        if (game->tech_tree.researched.empty())
        {
            tech_tree_log << "none\n";
        }
        else
        {
            for (auto &node : game->tech_tree.researched)
            {
                if (node != game->tech_tree.researched.back())
                {
                    tech_tree_log << node->name << ",";
                }
                else
                {
                    tech_tree_log << node->name << "\n";
                }
            }
        }

        tech_tree_log << "available,";
        if (game->tech_tree.available.empty())
        {
            tech_tree_log << "none\n";
        }
        else
        {
            for (auto &node : game->tech_tree.available)
            {
                if (node != game->tech_tree.available.back())
                {
                    tech_tree_log << node->name << ",";
                }
                else
                {
                    tech_tree_log << node->name << "\n";
                }
            }
        }

        tech_tree_log << "researching,";
        if (game->tech_tree.researching == nullptr)
        {
            tech_tree_log << "none\n";
        }
        else
        {
            tech_tree_log << game->tech_tree.researching->name << "\n";
        }

        tech_tree_log << "prev_researching,";
        if (game->tech_tree.prev_researching == nullptr)
        {
            tech_tree_log << "none\n";
        }
        else
        {
            tech_tree_log << game->tech_tree.prev_researching->name << "\n";
        }

        tech_tree_log << "remaining_time," << game->tech_tree.remaining_time << "\n";
    }
    tech_tree_log.close();
}

bool SaveDumper::save_game(std::string index, bool if_cover)
{
    // check if the save folder exists, if not, create it
    struct stat info;
    if (stat(folderpath.c_str(), &info) != 0)
    {
        if (mkdir(folderpath.c_str(), 0777) != 0)
        {
            throw std::runtime_error("Cannot create save folder");
        }
    }
    else if (!(info.st_mode & S_IFDIR))
    {
        throw std::runtime_error("Cannot create save folder");
    }
    
    //generate timestamp
    // std::time_t now = std::time(nullptr);
    // char timeStamp[20];
    // std::strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d_%H-%M-%S", std::localtime(&now));

    std::string sub_folderpath_by_time = folderpath + "game_" + index + "/";
    struct stat sub_info;
    
    if (!if_cover) {
        if (stat(sub_folderpath_by_time.c_str(), &sub_info) == 0 && (sub_info.st_mode & S_IFDIR))
        {
            std::string command = "rm -rf " + sub_folderpath_by_time;
            if (system(command.c_str()) != 0)
            {
            throw std::runtime_error("Failed to remove existing folder");
            }
        }
    }
    
    if (stat(sub_folderpath_by_time.c_str(), &sub_info) == 0 && (sub_info.st_mode & S_IFDIR))
    {
        return false; 
    }
    else if (mkdir(sub_folderpath_by_time.c_str(), 0777) != 0)
    {
        return false;
    }
    /*TODO: add hint to notify user slot is occupied*/
    
    save_game_general(sub_folderpath_by_time);
    save_attack_missile(sub_folderpath_by_time);
    save_cruise(sub_folderpath_by_time);
    save_city(sub_folderpath_by_time);
    save_tech_tree(sub_folderpath_by_time);
    return true;
}

std::vector<City> SaveLoader::load_cities()
{
    std::string filename = folderpath + "cities.txt";
    std::ifstream city_log(filename);
    if (!city_log.is_open())
    {
        throw std::runtime_error("Cannot open cities.txt");
    }
    std::vector<City> cities;
    std::vector<std::string> words;
    std::string line;
    std::string word;
    std::istringstream iss;
    std::getline(city_log, line);
    while (getline(city_log, line))
    {
        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        std::string name = words[0];
        Position position = Position(std::stoi(words[1]), std::stoi(words[2]));
        int hitpoint = std::stoi(words[3]);
        int base_productivity = std::stoi(words[4]);
        int productivity = std::stoi(words[5]);
        int cruise_num = std::stoi(words[6]);
        int countdown = std::stoi(words[7]);
        cities.push_back(City(position, name, hitpoint));
        cities.back().base_productivity = base_productivity;
        cities.back().productivity = productivity;
        cities.back().cruise_storage = cruise_num;
        cities.back().countdown = countdown;
    }
    city_log.close();
    return cities;
}

void SaveLoader::load_game_general(Game &game)
{
    std::string filename = folderpath + "general.txt";
    std::ifstream general_log(filename);
    if (!general_log.is_open())
    {
        throw std::runtime_error("Cannot open general.txt");
    }
    std::string line;
    std::string word;
    std::istringstream iss;
    while (getline(general_log, line))
    {
        if (line.empty())
        {
            general_log.close();
            return;
        }

        iss.clear();
        iss.str(line);
        getline(iss, word, ':');
        if (word == "level")
        {
            getline(iss, word);
            game.set_difficulty(std::stoi(word));
        }
        else if (word == "turn")
        {
            getline(iss, word);
            game.turn = std::stoi(word);
        }
        else if (word == "deposit")
        {
            getline(iss, word);
            game.deposit = std::stoi(word);
        }
        else if (word == "cursor_y")
        {
            getline(iss, word);
            game.cursor.y = std::stoi(word);
        }
        else if (word == "cursor_x")
        {
            getline(iss, word);
            game.cursor.x = std::stoi(word);
        }
        else if (word == "standard_bomb_counter")
        {
            getline(iss, word);
            game.standard_bomb_counter = std::stoi(word);
        }
        else if (word == "dirty_bomb_counter")
        {
            getline(iss, word);
            game.dirty_bomb_counter = std::stoi(word);
        }
        else if (word == "hydrogen_bomb_counter")
        {
            getline(iss, word);
            game.hydrogen_bomb_counter = std::stoi(word);
        }
        else if (word == "enemy_hitpoint")
        {
            getline(iss, word);
            game.enemy_hitpoint = std::stoi(word);
        }
        else if (word == "iron_curtain_activated")
        {
            getline(iss, word);
            game.iron_curtain_activated = std::stoi(word);
        }
        else if (word == "enhanced_radar_I")
        {
            getline(iss, word);
            game.en_enhanced_radar_I = std::stoi(word);
        }
        else if (word == "enhanced_radar_II")
        {
            getline(iss, word);
            game.en_enhanced_radar_II = std::stoi(word);
        }
        else if (word == "enhanced_radar_III")
        {
            getline(iss, word);
            game.en_enhanced_radar_III = std::stoi(word);
        }
        else if (word == "enhanced_cruise_I")
        {
            getline(iss, word);
            game.en_enhanced_cruise_I = std::stoi(word);
        }
        else if (word == "enhanced_cruise_II")
        {
            getline(iss, word);
            game.en_enhanced_cruise_II = std::stoi(word);
        }
        else if (word == "enhanced_cruise_III")
        {
            getline(iss, word);
            game.en_enhanced_cruise_III = std::stoi(word);
        }
        else if (word == "fortress_city")
        {
            getline(iss, word);
            game.en_fortress_city = std::stoi(word);
        }
        else if (word == "urgent_production")
        {
            getline(iss, word);
            game.en_urgent_production = std::stoi(word);
        }
        else if (word == "evacuated_industry")
        {
            getline(iss, word);
            game.en_evacuated_industry = std::stoi(word);
        }
        else if (word == "dirty_bomb")
        {
            getline(iss, word);
            game.en_dirty_bomb = std::stoi(word);
        }
        else if (word == "fast_nuke")
        {
            getline(iss, word);
            game.en_fast_nuke = std::stoi(word);
        }
        else if (word == "hydrogen_bomb")
        {
            getline(iss, word);
            game.en_hydrogen_bomb = std::stoi(word);
        }
        else if (word == "self_defense_sys")
        {
            getline(iss, word);
            game.en_self_defense_sys = std::stoi(word);
        }
        else if (word == "iron_curtain")
        {
            getline(iss, word);
            game.en_iron_curtain = std::stoi(word);
        }
        else if (word == "missile_manager_id")
        {
            getline(iss, word);
            game.missile_manager.id = std::stoi(word);
        }
    }
}

void SaveLoader::load_attack_missile(Game &game)
{
    std::string filename = folderpath + "atkmissiles.txt";
    std::ifstream atkmissile_log(filename);
    if (!atkmissile_log.is_open())
    {
        throw std::runtime_error("Cannot open atkmissiles.txt");
    }
    std::string line;
    std::string word;
    std::vector<std::string> words;
    std::istringstream iss;
    std::getline(atkmissile_log, line);
    while (getline(atkmissile_log, line))
    {
        if (line.empty())
        {
            atkmissile_log.close();
            return;
        }

        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        int id = std::stoi(words[0]);
        Position position = Position(std::stoi(words[1]), std::stoi(words[2]));
        Position target = Position(std::stoi(words[3]), std::stoi(words[4]));
        int damage = std::stoi(words[5]);
        int speed = std::stoi(words[6]);
        bool is_aimed = std::stoi(words[7]);

        // set target
        for (auto &city : game.cities)
        {
            if (city.get_position() == target)
            {
                game.missile_manager.create_attack_missile(position, city, damage, speed); //NOTE: this will add missile_manager's id by 1
                game.missile_manager.id--;
            }
        }
        // set is_aimed
        AttackMissile *attack_missile = dynamic_cast<AttackMissile *>(game.missile_manager.get_attack_missiles().back());
        attack_missile->is_aimed = is_aimed;
        //reset id
        game.missile_manager.missiles.back()->id = id;
    }
}

void SaveLoader::load_cruise(Game &game)
{
    std::string filename = folderpath + "cruise.txt";
    std::ifstream cruise_log(filename);
    if (!cruise_log.is_open())
    {
        throw std::runtime_error("Cannot open cruise.txt");
    }
    std::string line;
    std::string word;
    std::vector<std::string> words;
    std::istringstream iss;
    std::getline(cruise_log, line);
    while (getline(cruise_log, line))
    {
        if (line.empty())
        {
            cruise_log.close();
            return;
        }

        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        int id = std::stoi(words[0]);
        Position position = Position(std::stoi(words[1]), std::stoi(words[2]));
        int target_id = std::stoi(words[3]);
        int damage = std::stoi(words[4]);
        int speed = std::stoi(words[5]);

        // set target
        for (auto &missile : game.missile_manager.get_attack_missiles())
        {
            if (missile->id == target_id)
            {
                Missile *cruise_missile = new CruiseMissile(id, position, *missile, damage, speed, missile->id);
                game.missile_manager.missiles.push_back(cruise_missile);
                break;
            }
        }
    }
}

void SaveLoader::load_tech_tree(Game &game)
{
    std::string filename = folderpath + "tech_tree.txt";
    std::ifstream tech_tree_log(filename);
    if (!tech_tree_log.is_open())
    {
        throw std::runtime_error("Cannot open tech_tree.txt");
    }
    std::string line;
    std::string word;
    std::vector<std::string> words;
    std::istringstream iss;
    while (getline(tech_tree_log, line))
    {
        if (line.empty())
        {
            tech_tree_log.close();
            return;
        }

        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        if (words[0] == "researched")
        {
            if (words[1] == "none")
            {
                continue;
            }
            for (size_t i = 1; i < words.size(); ++i)
            {
                for (auto &node : game.tech_tree.nodes)
                {
                    if (node->name == words[i])
                    {
                        game.tech_tree.researched.push_back(node);
                        break;
                    }
                }
            }
        }

        if (words[0] == "available")
        {
            if (words[1] == "none")
            {
                continue;
            }
            for (size_t i = 1; i < words.size(); ++i)
            {
                for (auto &node : game.tech_tree.nodes)
                {
                    if (node->name == words[i])
                    {
                        game.tech_tree.available.push_back(node);
                        break;
                    }
                }
            }
        }

        if (words[0] == "researching")
        {
            if (words[1] == "none")
            {
                continue;
            }
            std::string researching_name = words[1];
            for (auto &node : game.tech_tree.nodes)
            {
                if (node->name == researching_name)
                {
                    game.tech_tree.researching = node;
                    break;
                }
            }
        }

        if (words[0] == "prev_researching")
        {
            if (words[1] == "none")
            {
                continue;
            }
            std::string prev_researching_name = words[1];
            for (auto &node : game.tech_tree.nodes)
            {
                if (node->name == prev_researching_name)
                {
                    game.tech_tree.prev_researching = node;
                    break;
                }
            }
        }

        if (words[0] == "remaining_time")
        {
            game.tech_tree.remaining_time = std::stoi(words[1]);
        }
    }
}

bool SaveLoader::load_game(Game &game, std::string index)
{
    folderpath = "../save/game_" + index + "/";
    
    struct stat info;
    if (stat(folderpath.c_str(), &info) != 0)
    {
        // throw std::runtime_error("Cannot open save folder");
        return false;
    }
    else if (!(info.st_mode & S_IFDIR))
    {
        // throw std::runtime_error("Cannot open save folder");
        return false;
    }
    
    game.reset();
    load_game_general(game);
    game.cities = load_cities();
    load_attack_missile(game);
    load_cruise(game);
    load_tech_tree(game);
    return true;
}
