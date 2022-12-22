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
    GLuint vboId{0};
    GLuint textureId{0};

    int width{256};
    int height{128};

    // for now, the shader needs to be public for all the classes that modify it
public:
    std::shared_ptr<Shader> shader = nullptr;

public:
    explicit Skybox(GLushort* texture);
    ~Skybox();

    void update(double dt);
    void draw();
};


#endif //PREDATION_SKYBOX_H
