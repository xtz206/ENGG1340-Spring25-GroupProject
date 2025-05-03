/**
 * @file saver.h
 * @brief Game state serialization/deserialization interface
 */

#ifndef SAVER_H
#define SAVER_H
#include <string>
#include <vector>
#include "game.h"

// forward declarations
class Game;
class City;

/**
 * @class AssetLoader
 * @brief Handles loading of game assets
 *
 * Responsible for parsing game resource files and constructing
 * initial game state objects. Currently implements hardcoded
 * paths for demonstration purposes.
 */
class AssetLoader
{
private:
    Game &game;

public:
    /**
     * @brief Construct a new AssetLoader object
     * @detail Default constructor provided for resource initialization
     * @param g Reference to the main game context
     */
    AssetLoader(Game &g) : game(g) {};
    void load_general(void);
    void load_background(void);
    void load_cities(void);
    std::vector<std::string> load_title(void);
    std::vector<std::vector<std::string>> load_video(void);
    void reset(void);
};

/**
 * @class GeneralChecker
 * @brief Checks if the game is running for the first time
 *
 * Provides functionality to determine if the game is being run for the first time
 * and saves a marker file to indicate that the game has been initialized.
 */
class GeneralChecker
{
private:
    std::string folderpath = "./";

public:
    GeneralChecker() {};
    bool is_first_run(void);
    void save_lastrun(void);
};

/**
 * @class SaveDumper
 * @brief Handles game state serialization to persistent storage
 *
 * Provides functionality to save complete game state to specified files,
 * organizing data into multiple logically separated files.
 */
class SaveDumper
{
private:
    Game &game;                       ///< Reference to main game context
    std::string folderpath = "save/"; ///< Base directory for save files

public:
    /**
     * @brief Construct a new SaveDumper bound to specific game instance
     * @param g Reference to active game context
     */
    SaveDumper(Game &g) : game(g) {};

    bool is_slot_empty(const std::string &savename);
    bool save_game(const std::string &savename);
    ///@name Component serializers
    ///@{
    void save_cities(const std::string &savepath);          ///< Save core game parameters
    void save_general(const std::string &savepath);         ///< Save offensive missile states
    void save_attack_missiles(const std::string &savepath); ///< Save cruise missile states
    void save_cruise_missiles(const std::string &savepath); ///< Save city configurations
    void save_tech_tree(const std::string &savepath);       ///< Save technology tree progress
    ///@}
};

/**
 * @class SaveLoader
 * @brief Handles game state deserialization from storage
 *
 * Implements loading mechanism that reconstructs game state from previously
 * saved data files, maintaining object relationships and dependencies.
 */
class SaveLoader
{
private:
    Game &game;                       ///< Reference to game context being loaded into
    std::string folderpath = "save/"; ///< Active loading directory path

public:
    /**
     * @brief Construct a new SaveLoader bound to target game instance
     * @param g Reference to game context for state reconstruction
     */
    SaveLoader(Game &g) : game(g) {};
    bool is_slot_empty(const std::string &savename);
    bool load_game(const std::string &savename);
    ///@name Component deserializers
    /// @{
    void load_cities(const std::string &savepath);          ///< Rebuild city objects
    void load_general(const std::string &savepath);         ///< Restore core game parameters
    void load_attack_missiles(const std::string &savepath); ///< Reconstruct attack missiles
    void load_cruise_missiles(const std::string &savepath); ///< Reconstruct cruise missiles
    void load_tech_tree(const std::string &savepath);       /// Restore technology tree state
    ///@}
};
#endif
