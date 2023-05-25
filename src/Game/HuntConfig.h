//
// Created by kkeiper1103 on 12/17/22.
//

#ifndef PREDATION_HUNTCONFIG_H
#define PREDATION_HUNTCONFIG_H

#include <vector>
#include <nuklear/nuklear.h>

struct HuntConfig {
    int selectedArea = 0;

    // available animals
    std::map<int, nk_bool> selectedAnimals;

    // available weapons
    std::map<int, nk_bool> selectedWeapons;

    int timeOfDay = 1;

    bool tranquilizer = false;
    bool observer = false;
};

#endif //PREDATION_HUNTCONFIG_H
