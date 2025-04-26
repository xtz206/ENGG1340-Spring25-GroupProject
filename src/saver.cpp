#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include "saver.h"
#include "game.h"

void Saver::save_game_general(std::string filepath)
{
    std::string filename = filepath + "general.txt";
    std::ofstream general_log(filename);
    if (general_log.is_open())
    {
        general_log << "turn: " << game -> get_turn() << "\n";
        general_log << "deposit: " << game -> get_deposit() << "\n";
        general_log << "cursor_y: " << game->cursor.y << "\n";
        general_log << "cursor_x: " << game->cursor.x << "\n";
        general_log << "counter_missile_cnt_down: " << game -> countdowns[0] << "\n";
        general_log << "dirty_bomb_cnt_down: " << game -> countdowns[1] << "\n";
        general_log << "hydron_bomb_cnt_down: " << game -> countdowns[2] << "\n";
        general_log << "counter_missile_num: " << game -> attack_missile_num[0] << "\n";
        general_log << "dirty_bomb_num: " << game -> attack_missile_num[1] << "\n";
        general_log << "hydron_bomb_num: " << game -> attack_missile_num[2] << "\n";
        general_log << "enemy_hp" << game -> missile_manager.hitpoint << "\n";
        general_log << "iron_curtain_activated: " << game -> iron_curtain_activated << "\n";
        general_log << "enhanced_radar_I: " << game -> en_enhanced_radar_I << "\n";
        general_log << "enhanced_radar_II: " << game -> en_enhanced_radar_II << "\n";
        general_log << "enhanced_radar_III: " << game -> en_enhanced_radar_III << "\n";
        general_log << "enhanced_cruise_I: " << game -> en_enhanced_cruise_I << "\n";
        general_log << "enhanced_cruise_II: " << game -> en_enhanced_cruise_II << "\n";
        general_log << "enhanced_cruise_III: " << game -> en_enhanced_cruise_III << "\n";
        general_log << "fortress_city: " << game -> en_fortress_city << "\n";
        general_log << "urgent_production: " << game -> en_urgent_production << "\n";
        general_log << "evacuated_industry: " << game -> en_evacuated_industry << "\n";
        general_log << "dirty_bomb: " << game -> en_dirty_bomb << "\n";
        general_log << "fast_nuke: " << game -> en_fast_nuke << "\n";
        general_log << "hydron_bomb: " << game -> en_hydrogen_bomb << "\n";
        general_log << "self_defense_sys: " << game -> en_self_defense_sys << "\n";
        general_log << "iron_curtain: "  << game -> en_iron_curtain  << "\n";
    }
    general_log.close();
}

void Saver::save_attack_missile(std::string filepath)
{
    std::string filename = filepath + "atkmissiles.txt";
    std::ofstream atkmissile_log(filename);
    if (atkmissile_log.is_open())
    {
        atkmissile_log << "id, y, x, target_y, target_x, damage, speed, is_aimed\n";
        for (auto &missile : game -> missile_manager.get_attack_missiles())
        {
            AttackMissile *attack_missile = dynamic_cast<AttackMissile *>(missile);
            atkmissile_log <<missile->id<<", "<<missile -> get_position().y << ", " << missile -> get_position().x << ", " << missile -> target.y << ", " << missile -> target.x << ", " << missile -> damage << ", " << missile -> speed <<", "<<attack_missile->is_aimed<<"\n";
        }
    }
    atkmissile_log.close();
}

void Saver::save_cruise(std::string filepath)
{
    std::string filename = filepath + "cruise.txt";
    std::ofstream cruise_log(filename);
    if (cruise_log.is_open())
    {
        for (auto &cruise : game -> missile_manager.get_cruise_missiles())
        {
                CruiseMissile *cruise_missile = dynamic_cast<CruiseMissile *>(cruise);
            cruise_log << "id, y, x, target_id, damage, speed\n";
            for (auto &missile : game -> missile_manager.get_cruise_missiles())
            {
                cruise_log << missile -> id << ", "<<missile -> get_position().y << ", " << missile -> get_position().x << ", " << cruise_missile->target_id<< ", "  << missile -> damage << ", " << missile -> speed << "\n";
        }
        }
    }
    cruise_log.close();
}

void Saver::save_city(std::string filepath)
{
    std::string filename = filepath + "cities.txt";
    std::ofstream city_log(filename);
    if (city_log.is_open())
    {
        city_log << "Name, y, x, hitpoint, base_productivity, productivity, cruise_num, cruise_build_time\n";
        for (const auto &city : game -> cities)
        {
            city_log << city.name << ", " << city.position.y << ", " << city.position.x << ", " 
            << city.hitpoint << ", " << city.base_productivity << ", " 
            <<city.productivity<<", "<<city.cruise_num << ", "<<city.cruise_build_time << "\n";
        }
    }
    city_log.close();
}

void Saver::save_tech_tree(std::string filepath)
{
    std::string filename = filepath + "tech_tree.txt";
    std::ofstream tech_tree_log(filename);
    if (tech_tree_log.is_open())
    {
        tech_tree_log << "researched, ";
        if (game -> tech_tree.researched.empty())
        {
            tech_tree_log << "none\n";
        }
        else
        {
            for (auto &node : game -> tech_tree.researched)
            {
                if (node != game->tech_tree.researched.back())
                {
                    tech_tree_log << node->name << ", ";
                }
                else
                {
                    tech_tree_log << node->name << "\n";
                }
            }
        }

        tech_tree_log << "available, ";
        if (game -> tech_tree.available.empty())
        {
            tech_tree_log << "none\n";
        }
        else
        {
            for (auto &node : game -> tech_tree.available)
            {
                if (node != game->tech_tree.available.back())
                {
                    tech_tree_log << node->name << ", ";
                }
                else
                {
                    tech_tree_log << node->name << "\n";
                }
            }
        }

        tech_tree_log << "researching, ";
        if (game -> tech_tree.researching == nullptr)
        {
            tech_tree_log << "none\n";
        }
        else
        {
            tech_tree_log << game -> tech_tree.researching->name << "\n";
        }

        tech_tree_log<< "prev_researching, ";
        if (game -> tech_tree.prev_researching == nullptr)
        {
            tech_tree_log << "none\n";
        }
        else
        {
            tech_tree_log << game -> tech_tree.prev_researching->name << "\n";
        }
        
        tech_tree_log << "remaining_time: "<< game -> tech_tree.remaining_time << "\n"; 
    }
    tech_tree_log.close();
}

void Saver::save_game()
{
    //check if the save folder exists, if not, create it
    struct stat info;
    if (stat(folderpath.c_str(), &info) != 0)
    {
        if (mkdir(folderpath.c_str(), 0777) != 0)
        {
            return;
        }
    }
    else if (!(info.st_mode & S_IFDIR))
    {
        return;
    }
    //generate timestamp
    std::time_t now = std::time(nullptr);
    char timeStamp[20];
    std::strftime(timeStamp, sizeof(timeStamp), "%Y-%m-%d_%H-%M-%S", std::localtime(&now));

    std::string sub_folderpath_by_time = folderpath + "game_" + timeStamp + "/";
    if (mkdir(sub_folderpath_by_time.c_str(), 0777) != 0)
    {
        return;
    }
    save_game_general(sub_folderpath_by_time);
    save_attack_missile(sub_folderpath_by_time);
    save_cruise(sub_folderpath_by_time);
    save_city(sub_folderpath_by_time);
    save_tech_tree(sub_folderpath_by_time);
}
