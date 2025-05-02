/**
 * @file saver.h
 * @brief Game state serialization/deserialization interface
 */
#ifndef SAVER_H
#define SAVER_H
#include <string>
#include <vector>
#include "game.h"

// Forward declarations
class Game;
class City;

/**
 * @class SaveDumper
 * @brief Handles game state serialization to persistent storage
 * 
 * Provides functionality to save complete game state to specified files,
 * organizing data into multiple logically separated files.
 */
class SaveDumper
{
    Game &game; ///< Reference to main game context
    std::string folderpath = "../save/"; ///< Base directory for save files

public:
    /**
     * @brief Construct a new SaveDumper bound to specific game instance
     * @param g Reference to active game context
     */
    SaveDumper(Game &g) : game(g) {}
     /**
     * @brief Execute full game save operation
     * @param index Save slot identifier (used for folder naming)
     * @param if_cover Overwrite existing save if true
     * @return true if save succeeded, false otherwise
     */
    bool save_game(std::string index, bool if_cover = false);
    ///@name Component serializers
    ///@{
    void save_game_general(std::string filepath);  ///< Save core game parameters
    void save_attack_missile(std::string filepath); ///< Save offensive missile states
    void save_cruise(std::string filepath);         ///< Save cruise missile states
    void save_city(std::string filepath);           ///< Save city configurations
    void save_tech_tree(std::string filepath);        ///< Save technology tree progress
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
    Game &game; ///< Reference to game context being loaded into
    // TODO: rename variable
    std::string dft_folderpath = "../save/";
    std::string folderpath; ///< Active loading directory path

public:
    /**
     * @brief Construct a new SaveLoader bound to target game instance
     * @param g Reference to game context for state reconstruction
     */
    SaveLoader(Game &g) : game(g), folderpath(dft_folderpath) {};
    ///@name Component deserializers 
    ///@{
    void load_cities();     ///< Rebuild city objects
    void load_general();    ///< Restore core game parameters
    void load_attack_missile(); ///< Reconstruct attack missiles
    void load_cruise();       ///< Reconstruct cruise missiles
    void load_tech_tree();   /// Restore technology tree state
    ///@}

    /**
     * @brief Execute complete game loading operation
     * @param index Save slot identifier to load
     * @return true if load succeeded, false otherwise
     * @throws std::runtime_error on critical parsing errors
     */
    bool load_game(const std::string &index);
};
#endif // SAVER_H