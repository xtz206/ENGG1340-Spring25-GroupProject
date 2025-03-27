#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>

#include "game.h"

class Loader
{
private:
    std::vector<std::string> content;

public:
    Loader(const std::string &filename);
    Coordinate load_size(void) const;
    std::vector<City> load_cities(void) const;
    std::vector<std::string> load_background(void) const;
};

#endif