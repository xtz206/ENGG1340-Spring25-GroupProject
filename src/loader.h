#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <vector>

#include "game.h"

class Loader
{
public:
    Loader(void) {};
    Position load_size(void) const;
    std::vector<City> load_cities(void) const;
    std::vector<std::string> load_background(void) const;
};

#endif
