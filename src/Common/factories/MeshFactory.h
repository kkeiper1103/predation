//
// Created by kkeiper1103 on 5/27/2023.
//

#ifndef PREDATION_MESHFACTORY_H
#define PREDATION_MESHFACTORY_H

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <ocarn2.h>
#include <physfs.h>

#include "IFactory.h"
#include "graphics/Mesh.h"

class MeshFactory : public Factory<Mesh> {
public:
    ~MeshFactory() override = default;

    MeshFactory::Ptr Get(const std::string &filename) override;
};

#endif //PREDATION_MESHFACTORY_H
