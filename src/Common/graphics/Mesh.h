#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include <easyloggingpp/easylogging++.h>
#include <physfs.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ocarn2.h>

class Mesh {
public:
    explicit Mesh() : Mesh({}, {}) {}
    Mesh(const std::vector<float>& positions, const std::vector<float>& texCoords);
    ~Mesh();


    void Debug() {

    }

protected:
    bool InitGL();
    bool UploadBuffers();
    bool UploadTextures();

public:
    GLuint id {0}, bufferIds[2] {0, 0}, textureId{0};

    std::vector<float> positions, texCoords;
};