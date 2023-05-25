//
// Created by kkeiper1103 on 5/25/2023.
//

#ifndef PREDATION_MAPSETTINGS_H
#define PREDATION_MAPSETTINGS_H

#include <string>

struct MapSettings {
    unsigned int seed = 0;

    std::string currentMap;
    std::string currentRsc;

    float elevation = .5;
    float temperature = .5;
    float vegetation = .5;
};

#endif //PREDATION_MAPSETTINGS_H
