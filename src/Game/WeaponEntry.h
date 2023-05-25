//
// Created by kkeiper1103 on 12/18/22.
//

#ifndef PREDATION_WEAPONENTRY_H
#define PREDATION_WEAPONENTRY_H

#include <string>
#include <ocarn2.h>

struct WeaponEntry {
    int price;

    std::string name;
    std::string modelFile;
    std::string bulletTgaFile;
};

#endif //PREDATION_WEAPONENTRY_H
