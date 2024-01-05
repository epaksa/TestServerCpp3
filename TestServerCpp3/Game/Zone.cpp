#include "Zone.h"
#include <fstream>

Zone::Zone(const std::string& map_file_name)
{
    std::ifstream stream(map_file_name);

    std::string string;

    while (std::getline(stream, string))
    {
        //std::cout << string << std::endl;
    }
}
