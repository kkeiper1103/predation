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
    Mesh(PHYSFS_File* file);
    Mesh(const std::string& filename);
    ~Mesh();

    bool IsLoaded() const { return dataPresent; }

    void Debug() {
        fprintf(stdout, "Model Name: %s\n", data.name);
    }

protected:
    bool InitGL();
    bool UploadBuffers();
    bool UploadTextures();

public:
    GLuint id {0}, bufferIds[2] {0, 0}, textureId{0};
    OCARN2::Mesh data;

protected:
    bool dataPresent = false;
};