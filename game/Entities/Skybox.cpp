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
    shader->setFloat("offset", SDL_GetTicks64() / 100000.f);
}

void Skybox::draw() {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);


    shader->use();
    shader->setInt("skybox", 0);

    // diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDepthMask(GL_TRUE);
}

struct SkyboxVertex {
    SkyboxVertex(float x, float y, float z): position(x, y, z), textureCoords() {}
    SkyboxVertex(float x, float y, float z, float s, float t) : position(x, y, z), textureCoords(s, t) {}

    glm::vec3 position;
    glm::vec2 textureCoords;
};

Skybox::Skybox(GLushort *texture) {
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenTextures(1, &textureId);


    auto verticesNew = generateVertices();

    std::vector<SkyboxVertex> vertices {
            // positions
            {-1.0f, 1.0f, -1.0f},
            {-1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},

            {-1.0f, -1.0f, 1.0f},
            {-1.0f, -1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, -1.0f},
            {-1.0f, 1.0f, 1.0f},
            {-1.0f, -1.0f, 1.0f},

            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},

            {-1.0f, -1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f},
            {-1.0f, -1.0f, 1.0f},

            {-1.0f, 1.0f, -1.0f},
            {1.0f, 1.0f, -1.0f},
            {1.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, 1.0f},
            {-1.0f, 1.0f, -1.0f},

            {-1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, -1.0f},
            {1.0f, -1.0f, -1.0f},
            {-1.0f, -1.0f, 1.0f},
            {1.0f, -1.0f, 1.0f}
    };

    glBindVertexArray(vaoId);
    {
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkyboxVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkyboxVertex), (void*) offsetof(SkyboxVertex, position));
    }
    glBindVertexArray(0);


    // create shader
    shader = Shader::FromFiles("resources/shaders/Skybox.vert", "resources/shaders/Skybox.frag");

    // load texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB5_A1, width, height, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, texture);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteBuffers(1, &vboId);
    glDeleteTextures(1, &textureId);
}

Skybox::VerticesReturnObject Skybox::generateVertices() {
    float radius = 64.f, widthSegments = 32, heightSegments = 16,
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
