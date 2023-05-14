//
// Created by kkeiper1103 on 12/20/22.
//

#include "Skybox.h"
#include "utils/convert_rgb5_a1_to_grayscale.h"
#include "glm/ext/matrix_transform.hpp"
#include "GameSettings.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>


void Skybox::update(double dt) {
    rotation.y += .5f * dt;

    model = glm::mat4(1.f);
    model = glm::rotate(model, glm::radians(rotation.y), {0, 1, 0});
}

void Skybox::draw() {
    shader->use();
    shader->setMat4("model", model);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    // diffuse map
    glBindTextures(0, 1, &textureId);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);
}

Skybox::Skybox(GLushort *texture) {
    glCreateVertexArrays(1, &vaoId);
    glCreateBuffers(4, buffers);
    glCreateTextures(GL_TEXTURE_2D, 1, &textureId);

    auto vertices = generateVertices();
    numElements = vertices.indices.size();


    glNamedBufferData(buffers[0], sizeof(GLuint) * vertices.indices.size(), vertices.indices.data(), GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vaoId, buffers[0]);

    glNamedBufferData(buffers[1], sizeof(GLfloat) * vertices.positions.size(), vertices.positions.data(), GL_STATIC_DRAW);
    glNamedBufferData(buffers[2], sizeof(GLfloat) * vertices.normals.size(), vertices.normals.data(), GL_STATIC_DRAW);
    glNamedBufferData(buffers[3], sizeof(GLfloat) * vertices.uvs.size(), vertices.uvs.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(vaoId, 0, buffers[1], 0, sizeof(GLfloat) * 3);
    glVertexArrayVertexBuffer(vaoId, 1, buffers[2], 0, sizeof(GLfloat) * 3);
    glVertexArrayVertexBuffer(vaoId, 2, buffers[3], 0, sizeof(GLfloat) * 2);

    glEnableVertexArrayAttrib(vaoId, 0);
    glVertexArrayAttribFormat(vaoId, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoId, 0, 0);

    glEnableVertexArrayAttrib(vaoId, 1);
    glVertexArrayAttribFormat(vaoId, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoId, 1, 1);

    glEnableVertexArrayAttrib(vaoId, 2);
    glVertexArrayAttribFormat(vaoId, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoId, 2, 2);

    // create shader
    shader = Shader::FromFiles("resources/shaders/Skybox.vert", "resources/shaders/Skybox.frag");

    // upscale texture
    auto upscaled = upscaleNearestNeighbor(texture);

    // load texture
    glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureStorage2D(textureId, 1, GL_RGB5_A1, width, height);
    glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(4, buffers);
    glDeleteTextures(1, &textureId);
}

Skybox::vertex_collection_t Skybox::generateVertices() {
    float radius = GameSettings::Get().viewRadius, widthSegments = 32, heightSegments = 32,
        phiStart = 0, phiLength = M_PI * 2, thetaStart = 0, thetaLength = M_PI;

    const float thetaEnd = std::min(thetaStart + thetaLength, (float) M_PI);

    int index = 0;
    std::vector< std::vector<GLuint> > grid;

    glm::vec3 vertex;

    // buffers
    std::vector<GLuint> indices;
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> uvs;

    // actually step through and generate the positions
    // generate vertices, normals and uvs
    for ( auto iy = 0; iy <= heightSegments; iy ++ ) {

        std::vector<GLuint> verticesRow {};

        const auto v = iy / heightSegments;

        // special case for the poles

        auto uOffset = 0;

        if ( iy == 0 && thetaStart == 0 ) {

            uOffset = 0.5 / widthSegments;

        } else if ( iy == heightSegments && thetaEnd == M_PI ) {

            uOffset = - 0.5 / widthSegments;

        }

        for ( auto ix = 0; ix <= widthSegments; ix ++ ) {

            const auto u = ix / widthSegments;

            // vertex
            vertex[0] = - radius * cos( phiStart + u * phiLength ) * sin( thetaStart + v * thetaLength );
            vertex[1] = radius * cos( thetaStart + v * thetaLength );
            vertex[2] = radius * sin( phiStart + u * phiLength ) * sin( thetaStart + v * thetaLength );

            positions.push_back( vertex[0] );
            positions.push_back( vertex[1] );
            positions.push_back( vertex[2] );

            // normal
            glm::vec3 normal = glm::normalize(vertex);

            normals.push_back( normal[0] );
            normals.push_back( normal[1] );
            normals.push_back( normal[2] );

            // uv

            uvs.push_back( u + uOffset);
            uvs.push_back( 1 - v );

            verticesRow.push_back( index ++ );

        }

        grid.push_back( verticesRow );

    }

    // indices
    for ( auto iy = 0; iy < heightSegments; iy ++ ) {

        for ( auto ix = 0; ix < widthSegments; ix ++ ) {

            auto a = grid[ iy ][ ix + 1 ];
            auto b = grid[ iy ][ ix ];
            auto c = grid[ iy + 1 ][ ix ];
            auto d = grid[ iy + 1 ][ ix + 1 ];

            if ( iy != 0 || thetaStart > 0 ) {
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(d);
            }

            if ( iy != heightSegments - 1 || thetaEnd < M_PI ) {
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }

        }

    }

    return {
        indices, positions, normals, uvs
    };
}
