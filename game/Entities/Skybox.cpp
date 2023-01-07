//
// Created by kkeiper1103 on 12/20/22.
//

#include "Skybox.h"
#include "utils/convert_rgb5_a1_to_grayscale.h"
#include "glm/ext/matrix_transform.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <stb/stb_image.h>
#include <SDL_timer.h>


template <typename T>
void BufferAttribute(int location, uint numComponents, const std::vector<T>& data) {
    if( data.size() % numComponents != 0 ) {
        LOG(ERROR) << "Buffer Size is not a Multiple of NumComponents!";
        return;
    }

    int currentlyBoundBuffer = 0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &currentlyBoundBuffer);

    if( currentlyBoundBuffer == 0 ) {
        LOG(ERROR) << "No Buffer Bound Currently! Can't BufferAttribute";
        return;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T) * data.size(), &data[0]);
    glVertexAttribPointer(location, numComponents, GL_FLOAT, GL_FALSE, sizeof(T) * numComponents, 0);
    glEnableVertexAttribArray(location);
};


void Skybox::update(double dt) {
    rotation.y += .5f * dt;

    model = glm::mat4(1.f);
    model = glm::rotate(model, glm::radians(rotation.y), {0, 1, 0});
}

void Skybox::draw() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    shader->use();
    shader->setMat4("model", model);

    // diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glBindVertexArray(vaoId);
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthMask(GL_TRUE);
}

Skybox::Skybox(GLushort *texture) {
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(4, buffers);
    glGenTextures(1, &textureId);


    auto vertices = generateVertices();
    numElements = vertices.indices.size();

    glBindVertexArray(vaoId);
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vertices.indices.size(), &vertices.indices[0], GL_STATIC_DRAW);


        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.positions.size(), &vertices.positions[0], GL_STATIC_DRAW);
        BufferAttribute<float>(0, 3, vertices.positions);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.normals.size(), &vertices.normals[0], GL_STATIC_DRAW);
        BufferAttribute<float>(1, 3, vertices.normals);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.uvs.size(), &vertices.uvs[0], GL_STATIC_DRAW);
        BufferAttribute<float>(2, 2, vertices.uvs);
    }
    glBindVertexArray(0);


    // create shader
    shader = Shader::FromFiles("resources/shaders/Skybox.vert", "resources/shaders/Skybox.frag");

    // load texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(4, buffers);
    glDeleteTextures(1, &textureId);
}

Skybox::VerticesReturnObject Skybox::generateVertices() {
    float radius = 128.f, widthSegments = 32, heightSegments = 16,
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
