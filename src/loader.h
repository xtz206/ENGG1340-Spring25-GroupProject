/**
 * @file loader.h
 * @brief Declares resource loading utilities for game initialization
 */
#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <vector>

#include "game.h"

/**
 * @class Loader
 * @brief Handles loading of game assets and configuration data
 * 
 * Responsible for parsing game resource files and constructing
 * initial game state objects. Currently implements hardcoded
 * paths for demonstration purposes.
 */
class Loader
{
public:
    /**
     * @brief Construct a new Loader object
     * @detail Default constructor provided for resource initialization
     */
    Loader(void) {};
    /**
     * @brief Load map dimensions from configuration
     * @return Position object containing map height/width
     * @throws std::runtime_error if size data is invalid
     * @detail Reads map size from predefined configuration file
     */
    Position load_size(void) const;
    /**
     * @brief Load city configurations
     * @return std::vector<City> List of initialized cities
     * @throws std::runtime_error if city data format is invalid
     * @detail Parses city positions, names and initial hitpoints
     */
    std::vector<City> load_cities(void) const;
    /**
     * @brief Load background terrain data
     * @return std::vector<std::string> ASCII representation of map
     * @throws std::runtime_error if background file not found
     * @detail Loads multi-line terrain pattern for map display
     */
    std::vector<std::string> load_background(void) const;
};

#endif
