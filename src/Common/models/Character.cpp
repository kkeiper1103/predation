//
// Created by kkeiper1103 on 5/28/2023.
//

#include "Character.h"

Character::Character(const std::string &filename) {
    data = load_car_file(filename.c_str());

    // generate glbuffer from car data
    std::vector<float> positions, texCoords;
    positions.resize(data.numFaces * 3);
    texCoords.resize(data.numFaces * 2);

    for(auto& face: data.faces) {
        auto v1 = data.vertices[face.v1],
                v2 = data.vertices[face.v2],
                v3 = data.vertices[face.v3];

        positions.emplace_back(v1.x); positions.emplace_back( v1.y); positions.emplace_back( v1.z); texCoords.emplace_back( face.tax / 256.f); texCoords.emplace_back( face.tay / 256.f);
        positions.emplace_back(v2.x); positions.emplace_back( v2.y); positions.emplace_back( v2.z); texCoords.emplace_back( face.tbx / 256.f); texCoords.emplace_back( face.tby / 256.f);
        positions.emplace_back(v3.x); positions.emplace_back( v3.y); positions.emplace_back( v3.z); texCoords.emplace_back( face.tcx / 256.f); texCoords.emplace_back( face.tcy / 256.f);
    }

    // I know we "should" use std::make_shared, but by using the actual alias, we don't have to update anything here if we change to unique_ptr in Mesh
    mesh = Mesh::Ptr { new Mesh(positions, texCoords) };


    
    // figure out how to handle the texture... :/
    std::vector<GLushort> texture(256 * 256, 0);
    memcpy(texture.data(), data.textureData, data.textureSize);

    //
    for(int i=0; i < data.textureSize / 2; i++) {
        auto& pixel = texture[i];

        int B = ((pixel>> 0) & 31);
        int G = ((pixel>> 5) & 31);
        int R = ((pixel>>10) & 31);
        int A = 1;
        if (!B && !G && !R)
            A = 0;

        texture[i] = (B) + (G<<5) + (R<<10) + (A<<15);
    }

    // diffuse (normal skin color) texture
    glTextureParameteri(mesh->textureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(mesh->textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(mesh->textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(mesh->textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(mesh->textureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(mesh->textureId, 1, GL_RGB5_A1, 256, 256);
    glTextureSubImage2D(mesh->textureId, 0, 0, 0, 256, 256, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture.data());
    glGenerateMipmap(GL_TEXTURE_2D);
}

Character::~Character() {
    free_mesh(data);
}
