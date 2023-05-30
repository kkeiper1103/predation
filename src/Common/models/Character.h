//
// Created by kkeiper1103 on 5/28/2023.
//

#ifndef PREDATION_CHARACTER_H
#define PREDATION_CHARACTER_H


#include <string>

#include "graphics/Mesh.h"
#include "graphics/Transform.h"

class Character {
public:
    explicit Character(const std::string& filename);
    ~Character();

protected:
    using Data = OCARN2::Mesh; // renaming ocarn2 type because it's confusing to see Mesh next to Mesh, when they're two different things (one is raw data, the other is GL specific)
    Data data;

    Mesh::Ptr mesh;
    Transform transform;
};


#endif //PREDATION_CHARACTER_H
