#include "Mesh.h"

#include <stdexcept>

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &id);
    glDeleteBuffers(2, bufferIds);
    glDeleteTextures(1, &textureId);

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

Mesh::Mesh(const std::vector<float> &positions, const std::vector<float> &texCoords) : positions{positions}, texCoords{texCoords} {

    InitGL();

    UploadBuffers();
    UploadTextures();
}
