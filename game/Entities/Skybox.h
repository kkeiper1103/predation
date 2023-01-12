//
// Created by kkeiper1103 on 12/20/22.
//

#ifndef PREDATION_SKYBOX_H
#define PREDATION_SKYBOX_H

#include <glad/glad.h>
#include "graphics/Shader.h"

class Skybox {
protected:
    GLuint vaoId{0};
    GLuint buffers[4] {0, 0, 0, 0};
    GLuint textureId{0};

    int width{256};
    int height{256};

    glm::vec3 rotation {0, 0, 0};
    glm::mat4 model {1.f};

    // for now, the shader needs to be public for all the classes that modify it
public:
    std::shared_ptr<Shader> shader = nullptr;

public:
    explicit Skybox(GLushort* texture);
    ~Skybox();

    void update(double dt);
    void draw();

protected:
    struct vertex_collection_t {
        std::vector<GLuint> indices;
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> uvs;
    };

    vertex_collection_t generateVertices();

    unsigned long numElements{0};
};


#endif //PREDATION_SKYBOX_H
