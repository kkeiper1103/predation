//
// Created by kkeiper1103 on 12/22/22.
//

#include "EntityMesh.h"

#include <memory>
#include "utils/convert_rgb5_a1_to_grayscale.h"

std::shared_ptr<Shader> EntityMesh::_shader = nullptr;

struct EntityMeshVertex {
    EntityMeshVertex(float x, float y, float z, float s, float t) : position(x, y, z), textureCoords(s, t) {};

    glm::vec3 position;
    glm::vec2 textureCoords;
};

EntityMesh::EntityMesh(const OCARN2::Mesh *const data) {
    strcpy(name, data->name);

    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenTextures(2, textures);

    std::vector<EntityMeshVertex> vertices;
    vertices.reserve(data->numFaces * 3);

    for(auto& face: data->faces) {
        auto v1 = data->vertices[face.v1],
             v2 = data->vertices[face.v2],
             v3 = data->vertices[face.v3];

        vertices.emplace_back(v1.x, v1.y, v1.z, face.tax / 256.f, face.tay / 256.f);
        vertices.emplace_back(v2.x, v2.y, v2.z, face.tbx / 256.f, face.tby / 256.f);
        vertices.emplace_back(v3.x, v3.y, v3.z, face.tcx / 256.f, face.tcy / 256.f);
    }
    vertexCount = data->numFaces * 3;


    glBindVertexArray(vaoId);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(EntityMeshVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EntityMeshVertex), (void*) offsetof(EntityMeshVertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(EntityMeshVertex), (void*) offsetof(EntityMeshVertex, textureCoords));
    }
    glBindVertexArray(0);

    // add the alpha channel, so we don't get black on the edge of the trees and stuff
    GLushort texture[256 * 256];
    memcpy(texture, data->textureData, data->textureSize);

    //
    for(int i=0; i < data->textureSize / 2; i++) {
        auto& pixel = texture[i];

        int B = ((pixel>> 0) & 31);
        int G = ((pixel>> 5) & 31);
        int R = ((pixel>>10) & 31);
        int A = 1;
        if (!B && !G && !R)
            A = 0;

        texture[i] = (B) + (G<<5) + (R<<10) + (A<<15);
    }

    // now, bind textures
    // diffuse texture
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, 256, 256, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    // no need for extraneous reset bind texture call

    // normal texture
    auto specularMap = to_grayscale(texture, 256, 256);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, 256, 256, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, &specularMap[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // lastly create shader
    if(_shader == nullptr)
        _shader = Shader::FromFiles("resources/shaders/Mesh.vert", "resources/shaders/Mesh.frag");

    shader = _shader;

    // set flag for loading complete
    ready = true;
}

EntityMesh::~EntityMesh() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
    glDeleteTextures(2, textures);
}