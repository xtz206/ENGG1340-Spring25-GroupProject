#include <fstream>
#include <sstream>

#include "loader.h"

/**
 * @brief Loads map dimensions from configuration file. 
 * Reads general.txt to extract height/width values using colon-separated format. Implements
 * file validation and format error handling through exception mechanisms.
 * @return Position Object containing map dimensions
 */
Position Loader::load_size(void) const
{
    // Attempt to open primary configuration file
    std::ifstream file("general.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open general.txt");  // Critical file missing
    }

    std::string line;
    try
    {
        // Parse height value from first line (format: "height:10")
        std::getline(file, line);
        int height = std::stoi(line.substr(line.find(':') + 1));  // Extract post-colon value
        
        // Parse width value from second line (format: "width:20")
        std::getline(file, line);
        int width = std::stoi(line.substr(line.find(':') + 1));
        return Position(height, width);  // Construct dimension object
    }
    catch (const std::exception &e)
    {
        file.close();  // Ensure clean resource release
        throw std::runtime_error("Invalid format in general.txt");  // Data parsing failed
    }
}
/**
 * @brief Loads city data from configuration file. 
 * Reads cities.txt to extract city attributes using comma-separated format. Implements
 * file validation and format error handling through exception mechanisms.
 * @return std::vector<City> Collection of City objects
 */
std::vector<City> Loader::load_cities(void) const
{
    std::ifstream file("cities.txt"); 
    if (!file.is_open()) // Validate file accessibility
    {
        throw std::runtime_error("Cannot open cities.txt");
    }

    std::vector<City> cities;
    std::string line;
    std::vector<std::string> words; // Temporary storage for CSV components
    std::string word;
    std::istringstream iss; // Reusable string stream for tokenization
    std::getline(file, line);
    while (getline(file, line)) // Process each data record
    {
        words.clear();
        iss.clear();
        iss.str(line);

        // Tokenize line using comma delimiter
        while (getline(iss, word, ','))
        {
            words.push_back(word);
        }
        // Convert coordinate values from string to integer
        Position position = Position(std::stoi(words[1]), std::stoi(words[2]));
        std::string name = words[0]; // Extract city identifier
        int hitpoint = std::stoi(words[3]);
        cities.push_back(City(position, name, hitpoint)); // Construct city instance
    }
    file.close(); // Release file handle
    return cities;
}

/**
 * @brief Loads battlefield background pattern from text file. 
 * Reads background.txt line-by-line, validates line count matches declared map height from
 * general.txt. Maintains original spatial formatting of background patterns.
 * @return std::vector<std::string> Lines representing battlefield visual layout
 */
std::vector<std::string> Loader::load_background(void) const
{
    Size size = load_size(); // Obtain preloaded map dimensions
    std::ifstream file("background.txt");
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open background.txt");
    }
    std::string line;
    std::vector<std::string> background; // Storage for pattern lines
    for (size_t y = 0; y < size.h; y++)
    {
        std::getline(file, line);
        background.push_back(line);
    }
    file.close();
    return background;
}
