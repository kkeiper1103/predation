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
    glNamedBufferData(bufferIds[0], sizeof(float) * positions.size(), positions.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(id, 0, bufferIds[0], 0, sizeof(float));

    glNamedBufferData(bufferIds[1], sizeof(float) * texCoords.size(), texCoords.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(id, 0, bufferIds[1], 0, sizeof(float));

    glEnableVertexArrayAttrib(id, 0);
    glVertexArrayAttribFormat(id, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(id, 0, 0);

    glEnableVertexArrayAttrib(id, 1);
    glVertexArrayAttribFormat(id, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(id, 1, 1);

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
