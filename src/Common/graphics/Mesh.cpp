#include "Mesh.h"

#include <stdexcept>

Mesh::Mesh(PHYSFS_File *file) {
    throw std::runtime_error("PHYSFS file based mesh not implemented yet! Requires retooling of OCARN2");
}

Mesh::Mesh(const std::string &filename) {
    data = load_car_file(filename.c_str()); dataPresent = true;

    InitGL();

    UploadBuffers();
    UploadTextures();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &id);
    glDeleteBuffers(2, bufferIds);
    glDeleteTextures(1, &textureId);

    free_mesh(data); dataPresent = false;
}

bool Mesh::InitGL() {
    glCreateVertexArrays(1, &id);
    glCreateBuffers(2, bufferIds);
    glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

    return true;
}

bool Mesh::UploadBuffers() {
    return true;
}

bool Mesh::UploadTextures() {
    return true;
}
