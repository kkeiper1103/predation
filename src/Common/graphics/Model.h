//
// Created by kkeiper1103 on 11/6/22.
//

#ifndef MODULETWO_MODEL_H
#define MODULETWO_MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

#include "Mesh.h"
#include "Shader.h"

class Model {
protected:
    std::shared_ptr<Mesh> mesh = nullptr;

    glm::mat4 model = glm::mat4(1.f);

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);

    float scale = 1.0;

    void computeModelMatrix();
public:
    explicit Model(std::shared_ptr<Mesh> mesh, const glm::vec3& position = {0, 0, 0}, const glm::vec3& rotation = {0, 0, 0}, float scale = 1.f);

    void update( double dt );
    void draw();
};



#endif //MODULETWO_MODEL_H
