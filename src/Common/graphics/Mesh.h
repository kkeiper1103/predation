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

#include "graphics/Texture.h"

class Mesh {
public:
    using Ptr = std::shared_ptr<Mesh>;

    explicit Mesh() : Mesh({}, {}) {}
    Mesh(const std::vector<float>& positions, const std::vector<float>& texCoords);
    ~Mesh();


    void Debug() {
        fprintf(stdout, "Vertices: %d\n", positions.size());
        for( int i = 0; i < positions.size(); i+=3 ) {
            fprintf(stdout, "\t[%.2f, %.2f, %.2f]\n", positions[i], positions[i+1], positions[i+2]);
        }

        fprintf(stdout, "TexCoords: %d\n", texCoords.size());
        for(int i=0; i < texCoords.size(); i += 2)
            fprintf(stdout, "\t[%.2f, %.2f]\n", texCoords[i], texCoords[i+1]);
    }

protected:
    bool InitGL();
    bool UploadBuffers();
    bool UploadTextures();

public:
    GLuint id {0}, bufferIds[2] {0, 0}, textureId{0};

    std::vector<float> positions, texCoords;
};