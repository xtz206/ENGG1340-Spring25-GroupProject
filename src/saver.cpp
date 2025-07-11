/**
 * @file saver.cpp
 * @brief This file contains the implementation of asset loading, saving, and game state management functionalities.
 *
 * The file provides methods to load and save various game assets and states, including general game settings,
 * background, cities, missiles, and the technology tree. It also includes functionality to check and handle
 * the first run of the program and manage save slots.
 *
 * Classes:
 * - AssetLoader: Handles loading of game assets such as general settings, background, cities, and title.
 * - GeneralChecker: Provides methods to check and save the first run state of the program.
 * - SaveDumper: Handles saving the game state to a specified folder, including general settings, cities, missiles, and the technology tree.
 * - SaveLoader: Handles loading the game state from a specified folder, including general settings, cities, missiles, and the technology tree.
 *
 * Functions:
 * - AssetLoader::load_general: Loads general game configuration from a file and initializes game settings.
 * - AssetLoader::load_background: Loads the background data from a file and stores it in the game's background container.
 * - AssetLoader::load_cities: Loads city data from a file and populates the game's city list.
 * - AssetLoader::load_title: Loads the content of the "title.txt" file into a vector of strings.
 * - AssetLoader::reset: Resets the game state by reloading assets and clearing game data.
 * - GeneralChecker::is_first_run: Checks if this is the first run of the program.
 * - GeneralChecker::save_lastrun: Creates a file named "lastrun" to indicate the first run.
 * - SaveDumper::is_slot_empty: Checks if the save slot is empty.
 * - SaveDumper::save_game: Saves the game state to a specified folder.
 * - SaveDumper::save_cities: Saves cities' states to a CSV file.
 * - SaveDumper::save_general: Saves general game state to a file.
 * - SaveDumper::save_attack_missiles: Saves attack missiles' states to a CSV file.
 * - SaveDumper::save_cruise_missiles: Saves cruise missiles' states to a CSV file.
 * - SaveDumper::save_tech_tree: Saves the technology tree state to a CSV file.
 * - SaveLoader::is_slot_empty: Checks if the save slot is empty.
 * - SaveLoader::load_game: Loads the game state from a specified folder.
 * - SaveLoader::load_cities: Loads the cities state from a specified folder.
 * - SaveLoader::load_general: Loads the general game state from a specified folder.
 * - SaveLoader::load_attack_missiles: Loads the attack missiles state from a specified folder.
 * - SaveLoader::load_cruise_missiles: Loads the cruise missiles state from a specified folder.
 * - SaveLoader::load_tech_tree: Loads the technology tree state from a specified folder.
 */

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "saver.h"
#include "game.h"

/**
 * @brief Loads general game configuration from a file and initializes game settings.
 *
 * This function reads the "general.txt" file, parses its contents, and updates
 * various game parameters such as size, cursor position, turn, score, counters,
 * and enabled features. Each line in the file is expected to follow the format:
 * "key:value". If the file cannot be opened, a runtime exception is thrown.
 *
 * @throws std::runtime_error If the "general.txt" file cannot be opened.
 */
void AssetLoader::load_general(void)
{
    // Attempt to open primary configuration file
    std::ifstream file("general.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open general.txt"); // Critical file missing
    }
    std::string line;
    std::string word;
    std::istringstream iss;
    while (getline(file, line))
    {
        if (line.empty())
        {
            file.close();
            return;
        }

        iss.clear();
        iss.str(line);
        getline(iss, word, ':');
        if (word == "size_y")
        {
            getline(iss, word);
            game.size.h = std::stoi(word);
            game.missile_manager.size.h = game.size.h;
        }
        else if (word == "size_x")
        {
            getline(iss, word);
            game.size.w = std::stoi(word);
            game.missile_manager.size.w = game.size.w;
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
        else if (word == "turn")
        {
            getline(iss, word);
            game.turn = std::stoi(word);
        }

        else if (word == "score")
        {
            getline(iss, word);
            game.score = std::stoi(word);
        }
        else if (word == "casualty")
        {
            getline(iss, word);
            game.casualty = std::stoi(word);
        }
        else if (word == "missile_manager_id")
        {
            getline(iss, word);
            game.missile_manager.id = std::stoi(word);
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
        else if (word == "iron_curtain_counter")
        {
            getline(iss, word);
            game.iron_curtain_counter = std::stoi(word);
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
    }
}

/**
 * @brief Loads the background data from a file and stores it in the game's background container.
 *
 * This function reads the contents of "background.txt" line by line and populates the
 * `game.background` vector with the data. If the file cannot be opened, it throws a runtime error.
 *
 * @throws std::runtime_error If the file "background.txt" cannot be opened.
 */
void AssetLoader::load_background(void)
{
    std::ifstream file("background.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open background.txt");
    }
    game.background.clear();
    for (std::string line; std::getline(file, line);)
    {
        game.background.push_back(line);
    }
    file.close();
}

/**
 * @brief Loads city data from a file and populates the game's city list.
 *
 * This function reads city information from a file named "cities.txt".
 * Each line in the file represents a city with its attributes separated by commas.
 * The attributes include the city's name, position (x, y), and hitpoints.
 * The function clears the existing city list in the game before loading new data.
 *
 * @throws std::runtime_error If the file "cities.txt" cannot be opened.
 */
void AssetLoader::load_cities(void)
{
    std::ifstream file("cities.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open cities.txt");
    }
    game.cities.clear();
    std::string line;
    std::vector<std::string> words;
    std::string word;
    std::istringstream iss;
    std::getline(file, line);
    while (getline(file, line))
    {
        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        Position position = Position(std::stoi(words[1]), std::stoi(words[2]));
        std::string name = words[0];
        int hitpoint = std::stoi(words[3]);
        game.cities.push_back(City(position, name, hitpoint));
    }
    file.close();
}

/**
 * @brief Loads the content of the "title.txt" file into a vector of strings.
 *
 * This function reads each line from the "title.txt" file and stores it in a
 * vector of strings. If the file cannot be opened, it throws a runtime error.
 *
 * @return std::vector<std::string> A vector containing each line of the file as a string.
 * @throws std::runtime_error If the "title.txt" file cannot be opened.
 */
std::vector<std::string> AssetLoader::load_title(void)
{
    std::ifstream file("title.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open title.txt");
    }
    std::vector<std::string> title;
    for (std::string line; std::getline(file, line);)
    {
        title.push_back(line);
    }
    file.close();
    return title;
}

/**
 * @brief Loads video frames from text files into a 2D vector of strings.
 *
 * This function reads a series of text files representing video frames
 * and stores their contents in a vector of frames, where each frame is
 * represented as a vector of strings (lines of text).
 *
 * @return std::vector<std::vector<std::string>>
 *         A 2D vector where each inner vector represents a video frame
 *         and contains strings corresponding to the lines of the frame.
 * @throws std::runtime_error If the video directory cannot be accessed.
 */
std::vector<std::vector<std::string>> AssetLoader::load_video(void)
{
    std::vector<std::vector<std::string>> frames;
    std::ifstream file;

    struct stat info;
    if (stat("video/", &info) != 0 || !(info.st_mode & S_IFDIR))
    {
        throw std::runtime_error("Cannot access video directory");
    }

    for (size_t index = 0;; index++)
    {
        file.open("video/frame" + std::to_string(index) + ".txt");
        if (!file.is_open())
        {
            break; // Stop when a file in the sequence is missing
        }
        std::vector<std::string> frame;
        for (std::string line; std::getline(file, line);)
        {
            frame.push_back(line);
        }
        frames.push_back(frame);
        file.close();
    }
    return frames;
}

/**
 * @brief Resets the game state by reloading assets and clearing game data.
 *
 * This function performs the following actions:
 * - Reloads general assets, background, and city data.
 * - Resets the missile manager's cities and clears all existing missiles.
 * - Resets the technology tree, including research progress and available technologies.
 * - Clears all feedback messages.
 */

void AssetLoader::reset(void)
{
    load_general();
    load_background();
    load_cities();
    game.missile_manager.cities = game.cities;
    for (auto missile : game.get_missiles())
    {
        delete missile;
    }
    game.missile_manager.missiles.clear();

    game.tech_tree.researching = nullptr;
    game.tech_tree.prev_researching = nullptr;
    game.tech_tree.remaining_time = 0;
    game.tech_tree.researched.clear();
    game.tech_tree.available.clear();

    game.feedbacks.clear();
}

/**
 * @brief Checks if this is the first run of the program.
 *
 * This function determines whether the program is being run for the first time
 * by checking for the existence of a specific file named "lastrun" in the
 * designated folder path. If the file exists and is a regular file, it indicates
 * that the program has been run before. Otherwise, it is considered the first run.
 *
 * @return true If the "lastrun" file does not exist, indicating the first run.
 * @return false If the "lastrun" file exists, indicating it is not the first run.
 */
bool GeneralChecker::is_first_run(void)
{
    std::string filepath = folderpath + "lastrun";
    struct stat info;
    if (stat(filepath.c_str(), &info) == 0 && (info.st_mode & S_IFREG))
    {
        // lastrun file exists, so this is not the first run
        return false;
    }
    // lastrun file does not exist, so this is the first run
    return true;
}

/**
 * @brief Creates a file named "lastrun" in the specified folder path to indicate the lastrun time.
 *
 * This function constructs a system command to create a file named "lastrun" in the directory
 * specified by the `folderpath` member variable. If the file creation fails, it throws a
 * runtime error.
 *
 * @throws std::runtime_error If the system command to create the "lastrun" file fails.
 */
void GeneralChecker::save_lastrun(void)
{
    std::string command = "touch " + folderpath + "lastrun";
    if (system(command.c_str()) != 0)
    {
        throw std::runtime_error("Failed to create lastrun file");
    }
}

/**
 * @brief Checks if the save slot is empty.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savename name of the save slot to check.
 * @return true if the slot is empty, false otherwise.
 */
bool SaveDumper::is_slot_empty(const std::string &savename)
{
    std::string savepath = folderpath + "game_" + savename + "/";
    struct stat saveinfo;
    if (stat(savepath.c_str(), &saveinfo) == 0 && (saveinfo.st_mode & S_IFDIR))
    {
        return false;
    }
    return true;
}

/**
 * @brief Saves the game state to a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * Then calls the save functions for general game state, attack missiles,
 * cruise missiles, cities, and technology tree.
 * @param savename Index of the save slot to save.
 * @return true if the game is saved successfully, false otherwise.
 */
bool SaveDumper::save_game(const std::string &savename)
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

    std::string savepath = folderpath + "game_" + savename + "/";
    struct stat saveinfo;

    if (!is_slot_empty(savename))
    {
        // remove the existing folder
        std::string command = "rm -rf " + savepath;
        if (system(command.c_str()) != 0)
        {
            throw std::runtime_error("Failed to remove existing folder");
        }
    }
    if (mkdir(savepath.c_str(), 0777) != 0)
    {
        throw std::runtime_error("Cannot create save folder");
    }

    save_general(savepath);
    save_attack_missiles(savepath);
    save_cruise_missiles(savepath);
    save_cities(savepath);
    save_tech_tree(savepath);
    return true;
}

/**
 * @brief Saves cities' states to a csv file.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savepath Path to the save directory.
 */
void SaveDumper::save_cities(const std::string &savepath)
{
    std::string filename = savepath + "cities.txt";
    std::ofstream city_log(filename);
    if (city_log.is_open())
    {
        city_log << "Name,y,x,hitpoint,base_productivity,productivity,cruise_storage,countdown\n";
        for (auto &city : game.cities)
        {
            city_log << city.name << "," << city.position.y << "," << city.position.x << ","
                     << city.hitpoint << "," << city.base_productivity << ","
                     << city.productivity << "," << city.cruise_storage << "," << city.countdown << "\n";
        }
    }
    city_log.close();
}

/**
 * @brief Save general game state to a file.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savepath Path to the save directory.
 */
void SaveDumper::save_general(const std::string &savepath)
{
    std::string filename = savepath + "general.txt";
    std::ofstream general_log(filename);
    if (general_log.is_open())
    {
        // NOTE: game info
        general_log << "size_y:" << game.size.h << "\n";
        general_log << "size_x:" << game.size.w << "\n";
        general_log << "cursor_y:" << game.cursor.y << "\n";
        general_log << "cursor_x:" << game.cursor.x << "\n";
        general_log << "turn:" << game.get_turn() << "\n";
        general_log << "deposit:" << game.get_deposit() << "\n";
        general_log << "difficulty_level:" << game.difficulty_level << "\n";
        general_log << "enemy_hitpoint:" << game.enemy_hitpoint << "\n";
        general_log << "score:" << game.get_score() << "\n";
        general_log << "casualty:" << game.get_casualty() << "\n";
        general_log << "missile_manager_id:" << game.missile_manager.id << "\n";

        // NOTE: super weapon
        general_log << "standard_bomb_counter:" << game.standard_bomb_counter << "\n";
        general_log << "dirty_bomb_counter:" << game.dirty_bomb_counter << "\n";
        general_log << "hydrogen_bomb_counter:" << game.hydrogen_bomb_counter << "\n";
        general_log << "iron_curtain_counter:" << game.iron_curtain_counter << "\n";

        // NOTE: technology
        general_log << "enhanced_radar_I:" << game.en_enhanced_radar_I << "\n";
        general_log << "enhanced_radar_II:" << game.en_enhanced_radar_II << "\n";
        general_log << "enhanced_radar_III:" << game.en_enhanced_radar_III << "\n";
        general_log << "enhanced_cruise_I:" << game.en_enhanced_cruise_I << "\n";
        general_log << "enhanced_cruise_II:" << game.en_enhanced_cruise_II << "\n";
        general_log << "enhanced_cruise_III:" << game.en_enhanced_cruise_III << "\n";
        general_log << "fortress_city:" << game.en_fortress_city << "\n";
        general_log << "urgent_production:" << game.en_urgent_production << "\n";
        general_log << "evacuated_industry:" << game.en_evacuated_industry << "\n";
        general_log << "dirty_bomb:" << game.en_dirty_bomb << "\n";
        general_log << "fast_nuke:" << game.en_fast_nuke << "\n";
        general_log << "hydrogen_bomb:" << game.en_hydrogen_bomb << "\n";
        general_log << "self_defense_sys:" << game.en_self_defense_sys << "\n";
        general_log << "iron_curtain:" << game.en_iron_curtain << "\n";
    }
    general_log.close();
}

/**
 * @brief Saves attack missiles' states to a csv file.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savepath Path to the save directory.
 */
void SaveDumper::save_attack_missiles(const std::string &savepath)
{
    std::string filename = savepath + "attack_missiles.txt";
    std::ofstream attack_missile_log(filename);
    if (attack_missile_log.is_open())
    {
        attack_missile_log << "id,y,x,target_y,target_x,damage,speed,is_aimed" << "\n";
        for (auto &missile : game.missile_manager.get_attack_missiles())
        {
            AttackMissile *attack_missile = dynamic_cast<AttackMissile *>(missile);
            attack_missile_log << missile->id << "," << missile->get_position().y << "," << missile->get_position().x << "," << missile->target.y << "," << missile->target.x << "," << missile->damage << "," << missile->speed << "," << attack_missile->is_aimed << "\n";
        }
    }
    attack_missile_log.close();
}

/**
 * @brief Saves cruise missiles' states to a csv file.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savepath Path to the save directory.
 */
void SaveDumper::save_cruise_missiles(const std::string &savepath)
{
    std::string filename = savepath + "cruise_missiles.txt";
    std::ofstream cruise_missile_log(filename);
    if (cruise_missile_log.is_open())
    {
        cruise_missile_log << "id,y,x,target_id,damage,speed" << "\n";

        for (auto missile : game.missile_manager.get_cruise_missiles())
        {
            CruiseMissile *cruise_missile = dynamic_cast<CruiseMissile *>(missile);
            cruise_missile_log << missile->id << "," << missile->get_position().y << "," << missile->get_position().x << "," << cruise_missile->target_id << "," << missile->damage << "," << missile->speed << "\n";
        }
    }
    cruise_missile_log.close();
}

/**
 * @brief Saves the technology tree state to a csv file.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savepath Path to the save directory.
 */
void SaveDumper::save_tech_tree(const std::string &savepath)
{
    std::string filename = savepath + "tech_tree.txt";
    std::ofstream tech_tree_log(filename);
    if (tech_tree_log.is_open())
    {
        tech_tree_log << "researched,";
        if (game.tech_tree.researched.empty())
        {
            tech_tree_log << "none" << "\n";
        }
        else
        {
            for (auto &node : game.tech_tree.researched)
            {
                if (node != game.tech_tree.researched.back())
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
        if (game.tech_tree.available.empty())
        {
            tech_tree_log << "none" << "\n";
        }
        else
        {
            for (auto node : game.tech_tree.available)
            {
                if (node != game.tech_tree.available.back())
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
        if (game.tech_tree.researching == nullptr)
        {
            tech_tree_log << "none" << "\n";
        }
        else
        {
            tech_tree_log << game.tech_tree.researching->name << "\n";
        }

        tech_tree_log << "prev_researching,";
        if (game.tech_tree.prev_researching == nullptr)
        {
            tech_tree_log << "none" << "\n";
        }
        else
        {
            tech_tree_log << game.tech_tree.prev_researching->name << "\n";
        }

        tech_tree_log << "remaining_time," << game.tech_tree.remaining_time << "\n";
    }
    tech_tree_log.close();
}

/**
 * @brief Checks if the save slot is empty.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. If no save file exists, it creates a new save directory.
 * @param savename name of the save slot to check.
 * @return true if the slot is empty, false otherwise.
 */
bool SaveLoader::is_slot_empty(const std::string &savename)
{
    std::string savepath = folderpath + "game_" + savename + "/";
    struct stat saveinfo;
    if (stat(savepath.c_str(), &saveinfo) == 0 && (saveinfo.st_mode & S_IFDIR))
    {
        return false;
    }
    return true;
}

/**
 * @brief Loads the game state from a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. Then calls the load functions for general game state, attack missiles,
 * cruise missiles, cities, and technology tree.
 * @param savename Index of the save slot to load.
 * @return true if the game is loaded successfully, false otherwise.
 */
bool SaveLoader::load_game(const std::string &savename)
{
    std::string savepath = folderpath + "game_" + savename + "/";

    struct stat info;
    if (stat(folderpath.c_str(), &info) != 0)
    {
        return false;
    }
    else if (!(info.st_mode & S_IFDIR))
    {
        return false;
    }

    load_general(savepath);
    load_cities(savepath);
    load_attack_missiles(savepath);
    load_cruise_missiles(savepath);
    load_tech_tree(savepath);
    return true;
}

/**
 * @brief Loads the cities state from a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. Then reads the cities data from the file and populates the game state.
 * If the file cannot be opened, throws a runtime error.
 * @param savepath Path to the save directory.
 * @throws std::runtime_error if the file cannot be opened.
 *
 */
void SaveLoader::load_cities(const std::string &savepath)
{
    std::string filename = savepath + "cities.txt";
    std::ifstream city_log(filename);
    if (!city_log.is_open())
    {
        throw std::runtime_error("Cannot open cities.txt");
    }

    std::vector<std::string> words;
    std::string line;
    std::string word;
    std::istringstream iss;

    game.cities.clear();
    std::getline(city_log, line); // NOTE: skip field names
    while (getline(city_log, line))
    {
        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        std::string name = words.at(0);
        Position position = Position(std::stoi(words.at(1)), std::stoi(words.at(2)));
        int hitpoint = std::stoi(words.at(3));
        int base_productivity = std::stoi(words.at(4));
        int productivity = std::stoi(words.at(5));
        int cruise_num = std::stoi(words.at(6));
        int countdown = std::stoi(words.at(7));
        game.cities.push_back(City(position, name, hitpoint));
        game.cities.back().base_productivity = base_productivity;
        game.cities.back().productivity = productivity;
        game.cities.back().cruise_storage = cruise_num;
        game.cities.back().countdown = countdown;
    }
    city_log.close();
}

/**
 * @brief Loads the general game state from a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. Then reads the general game state data from the file and populates
 * the game state. If the file cannot be opened, throws a runtime error.
 * @param savepath Path to the save directory.
 * @throws std::runtime_error if the file cannot be opened.
 *
 */
void SaveLoader::load_general(const std::string &savepath)
{
    std::string filename = savepath + "general.txt";
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
        if (word == "size_y")
        {
            getline(iss, word);
            game.size.h = std::stoi(word);
            game.missile_manager.size.h = game.size.h;
        }
        else if (word == "size_x")
        {
            getline(iss, word);
            game.size.w = std::stoi(word);
            game.missile_manager.size.w = game.size.w;
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
        else if (word == "difficulty_level")
        {
            getline(iss, word);
            game.difficulty_level = std::stoi(word);
            game.missile_manager.set_difficulty(std::stoi(word));
        }
        else if (word == "enemy_hitpoint")
        {
            getline(iss, word);
            game.enemy_hitpoint = std::stoi(word);
        }
        else if (word == "score")
        {
            getline(iss, word);
            game.score = std::stoi(word);
        }
        else if (word == "casualty")
        {
            getline(iss, word);
            game.casualty = std::stoi(word);
        }
        else if (word == "missile_manager_id")
        {
            getline(iss, word);
            game.missile_manager.id = std::stoi(word);
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
        else if (word == "iron_curtain_counter")
        {
            getline(iss, word);
            game.iron_curtain_counter = std::stoi(word);
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
    }
}

/**
 * @brief Loads the attack missiles state from a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. Then reads the attack missiles data from the file and populates
 * the game state. If the file cannot be opened, throws a runtime error.
 * @param savepath Path to the save directory.
 * @throws std::runtime_error if the file cannot be opened.
 *
 */
void SaveLoader::load_attack_missiles(const std::string &savepath)
{
    std::string filename = savepath + "attack_missiles.txt";
    std::ifstream attack_missile_log(filename);
    if (!attack_missile_log.is_open())
    {
        throw std::runtime_error("Cannot open attack_missiles.txt");
    }
    std::string line;
    std::string word;
    std::vector<std::string> words;
    std::istringstream iss;
    std::getline(attack_missile_log, line);
    while (getline(attack_missile_log, line))
    {
        if (line.empty())
        {
            attack_missile_log.close();
            return;
        }

        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        int id = std::stoi(words.at(0));
        Position position = Position(std::stoi(words.at(1)), std::stoi(words.at(2)));
        Position target = Position(std::stoi(words.at(3)), std::stoi(words.at(4)));
        int damage = std::stoi(words.at(5));
        int speed = std::stoi(words.at(6));
        bool is_aimed = std::stoi(words.at(7));

        for (auto &city : game.cities)
        {
            if (city.get_position() == target)
            {
                // NOTE: this will add missile_manager's id by 1
                AttackMissile *attack_missile = new AttackMissile(id, position, city, damage, speed);
                attack_missile->is_aimed = is_aimed;
                game.missile_manager.missiles.push_back(attack_missile);
            }
        }
    }
}

/**
 * @brief Loads the cruise missiles state from a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. Then reads the cruise missiles data from the file and populates
 * the game state. If the file cannot be opened, throws a runtime error.
 * @param savepath Path to the save directory.
 * @throws std::runtime_error if the file cannot be opened.
 *
 */
void SaveLoader::load_cruise_missiles(const std::string &savepath)
{
    std::string filename = savepath + "cruise_missiles.txt";
    std::ifstream cruise_missile_log(filename);
    if (!cruise_missile_log.is_open())
    {
        throw std::runtime_error("Cannot open cruise_missiles.txt");
    }
    std::string line;
    std::string word;
    std::vector<std::string> words;
    std::istringstream iss;
    std::getline(cruise_missile_log, line);
    while (getline(cruise_missile_log, line))
    {
        if (line.empty())
        {
            cruise_missile_log.close();
            return;
        }

        words.clear();
        iss.clear();
        iss.str(line);

        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }

        int id = std::stoi(words.at(0));
        Position position = Position(std::stoi(words.at(1)), std::stoi(words.at(2)));
        int target_id = std::stoi(words.at(3));
        int damage = std::stoi(words.at(4));
        int speed = std::stoi(words.at(5));

        for (auto &missile : game.missile_manager.get_attack_missiles())
        {
            if (missile->id == target_id)
            {
                CruiseMissile *cruise_missile = new CruiseMissile(id, position, *missile, damage, speed, missile->id);
                game.missile_manager.missiles.push_back(cruise_missile);
                break;
            }
        }
    }
}

/**
 * @brief Loads the technology tree state from a specified folder.
 * Validates the existence of the save directory and checks for the presence of
 * a save file. Then reads the technology tree data from the file and populates
 * the game state. If the file cannot be opened, throws a runtime error.
 * @param savepath Path to the save directory.
 * @throws std::runtime_error if the file cannot be opened.
 *
 */
void SaveLoader::load_tech_tree(const std::string &savepath)
{
    std::string filename = savepath + "tech_tree.txt";
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

        if (words.at(0) == "researched")
        {
            if (words.at(1) == "none")
            {
                continue;
            }
            for (size_t i = 1; i < words.size(); ++i)
            {
                for (auto &node : game.tech_tree.nodes)
                {
                    if (node->name == words.at(i))
                    {
                        game.tech_tree.researched.push_back(node);
                        break;
                    }
                }
            }
        }

        if (words.at(0) == "available")
        {
            if (words.at(1) == "none")
            {
                continue;
            }
            for (size_t i = 1; i < words.size(); ++i)
            {
                for (auto &node : game.tech_tree.nodes)
                {
                    if (node->name == words.at(i))
                    {
                        game.tech_tree.available.push_back(node);
                        break;
                    }
                }
            }
        }

        if (words.at(0) == "researching")
        {
            if (words.at(1) == "none")
            {
                continue;
            }
            std::string researching_name = words.at(1);
            for (auto &node : game.tech_tree.nodes)
            {
                if (node->name == researching_name)
                {
                    game.tech_tree.researching = node;
                    break;
                }
            }
        }

        if (words.at(0) == "prev_researching")
        {
            if (words.at(1) == "none")
            {
                continue;
            }
            std::string prev_researching_name = words.at(1);
            for (auto &node : game.tech_tree.nodes)
            {
                if (node->name == prev_researching_name)
                {
                    game.tech_tree.prev_researching = node;
                    break;
                }
            }
        }

        if (words.at(0) == "remaining_time")
        {
            game.tech_tree.remaining_time = std::stoi(words.at(1));
        }
    }
}
