//
// Created by kkeiper1103 on 11/5/22.
//

#ifndef MODULETWO_VERTEX_H
#define MODULETWO_VERTEX_H

#include <glm/glm.hpp>

struct Vertex {
    Vertex(float x, float y, float z) : position(x, y, z) {}
    Vertex(float x, float y, float z, float s, float t) : position(x, y, z), texture(s, t) {}
    glm::vec3 position{};
    glm::vec2 texture{};
    glm::vec3 color{};
};

#endif //MODULETWO_VERTEX_H
