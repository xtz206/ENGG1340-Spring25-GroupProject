#include <fstream>

#include "loader.h"

Loader::Loader(const std::string &filename)
{
    std::ifstream file;
    std::string line;

    file.open(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open " + filename);
    }

    while (std::getline(file, line))
    {
        content.push_back(line);
    }

    file.close();
}

Coordinate Loader::load_size(void) const
{
    Coordinate size = {0, 0};
    size.first = std::stoi(content[0].substr(content[0].find(":") + 1));
    size.second = std::stoi(content[1].substr(content[1].find(":") + 1));
    return size;
}

std::vector<City> Loader::load_cities(void) const
{
    std::vector<City> cities;
    int count = std::stoi(content[2].substr(content[2].find(":") + 1));
    for (size_t index = 3; index < 3 + count; index++)
    {

        Coordinate position;
        std::string name;
        int hitpoint;

        std::string line = content[index];
        size_t pos;

        pos = line.find(", ");
        name = line.substr(0, pos);
        line = line.substr(pos + 2);

        pos = line.find(", ");
        position.first = std::stoi(line.substr(0, pos));
        line = line.substr(pos + 2);

        pos = line.find(", ");
        position.second = std::stoi(line.substr(0, pos));
        line = line.substr(pos + 2);

        pos = line.find(", ");
        hitpoint = std::stoi(line.substr(0, pos));

        cities.push_back(City(position, name, hitpoint));
    }
    return cities;
}

std::vector<std::string> Loader::load_background(void) const
{
    std::vector<std::string> background;
    for (size_t index = 4 + std::stoi(content[2].substr(content[2].find(":") + 1)); index < content.size(); index++)
    {
        background.push_back(content[index]);
    }
    return background;
}