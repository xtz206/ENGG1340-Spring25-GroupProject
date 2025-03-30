#include <fstream>
#include <sstream>

#include "loader.h"

Position Loader::load_size(void) const
{
    std::ifstream file("general.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open general.txt");
    }
    std::string line;
    try
    {
        std::getline(file, line);
        int height = std::stoi(line.substr(line.find(':') + 1));
        std::getline(file, line);
        int width = std::stoi(line.substr(line.find(':') + 1));
        return Position(height, width);
    }

    catch (const std::exception &e)
    {
        file.close();
        throw std::runtime_error("Invalid format in general.txt");
    }
}

std::vector<City> Loader::load_cities(void) const
{
    std::ifstream file("cities.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open cities.txt");
    }

    std::vector<City> cities;
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
        cities.push_back(City(position, name, hitpoint));
    }
    file.close();
    return cities;
}

std::vector<std::string> Loader::load_background(void) const
{
    Size size = load_size();
    std::ifstream file("background.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open background.txt");
    }
    std::string line;
    std::vector<std::string> background;
    for (size_t y = 0; y < size.h; y++)
    {
        std::getline(file, line);
        background.push_back(line);
    }
    file.close();
    return background;
}
