//
// Created by kkeiper1103 on 12/20/22.
//

#include "Skybox.h"
#include "utils/convert_rgb5_a1_to_grayscale.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <stb/stb_image.h>
#include <SDL_timer.h>

void Skybox::update(double dt) {

}

void Skybox::draw() {
    shader->use();
    shader->setInt("skybox", 0);
    shader->setInt("normalMap", 1);
    shader->setFloat("offset", SDL_GetTicks64() / 100000.f);

    // diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    // normal map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

struct SkyboxVertex {
    SkyboxVertex(float x, float y, float z, float s, float t) : position(x, y, z), textureCoords(s, t) {}

    glm::vec3 position;
    glm::vec2 textureCoords;
};

Skybox::Skybox(GLushort *texture) {
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenTextures(2, textures);

    std::vector<SkyboxVertex> vertices {
        { -100.f, -100.f, 10.f, 0.f, 0.f},
        {-100.f, 100.f, 10.f, 0.f, 1.f},
        {100.f, 100.f, 10.f, 1.f, 1.f},

        {-100.f, -100.f, 10.f, 0.f, 0.f},
        {100.f, 100.f, 10.f, 1.f, 1.f},
        {100.f, -100.f, 10.f, 1.f, 0.f}
    };

    glBindVertexArray(vaoId);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkyboxVertex), (void*) offsetof(SkyboxVertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SkyboxVertex), (void*) offsetof(SkyboxVertex, textureCoords));
    }
    glBindVertexArray(0);


    // create shader
    shader = Shader::FromFiles("resources/shaders/Skybox.vert", "resources/shaders/Skybox.frag");

    // load texture
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // gonna convert to grayscale for normal mapping
    auto normals = to_grayscale(texture, 256, 256);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, &normals[0]);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
    glDeleteTextures(2, textures);
}
