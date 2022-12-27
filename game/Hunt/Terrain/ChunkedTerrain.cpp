//
// Created by kkeiper1103 on 12/18/22.
//

#include <cstring>
#include <cmath>

#include "ChunkedTerrain.h"

ChunkedTerrain::ChunkedTerrain(OCARN2::Map *map, OCARN2::Rsc* rsc, int chunksPerSide) : map{map}, rsc{rsc}, chunksPerSide{chunksPerSide} {

    // pre-allocate the memory by creating empty chunks
    chunks.resize(chunksPerSide);
    for(int z=0; z < chunksPerSide; z++) {
        chunks[z].resize(chunksPerSide);
    }

    // now, fill the chunks
    for(int z=0; z < chunksPerSide; z++) {
        for(int x = 0; x < chunksPerSide; x++) {
            auto& chunk = chunks[z][x];

            auto data = GetRegion(x, z, 1024 / chunksPerSide);
            chunk.data = data;
            chunk.vertexCount = data.size();

            glGenVertexArrays(1, &chunk.vaoId);
            glBindVertexArray(chunk.vaoId);
            {
                glGenBuffers(1, &chunk.bufferId);
                glBindBuffer(GL_ARRAY_BUFFER, chunk.bufferId);
                glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * data.size(), &data[0], GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*) offsetof(TerrainVertex, position));
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*) offsetof(TerrainVertex, texture));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*) offsetof(TerrainVertex, normal));
                glEnableVertexAttribArray(2);
            }
            glBindVertexArray(0);
        }
    }

    //
    UploadTexture();

    // create shader for class
    shader = Shader::FromFiles("resources/shaders/ChunkedTerrain.vert", "resources/shaders/ChunkedTerrain.frag");

    // set time of day lighting?
    // shouldn't this really be a ubo, not an individual uniform for terrain?
    shader->setVec3("lights[0].diffuse", { .5, .5, .5 });
    shader->setVec3("lights[0].ambient", {.1, .1, .1});
    shader->setVec3("lights[0].specular", { 1, 1, 1 });
    shader->setFloat("lights[0].attenuation", .8);
}

ChunkedTerrain::~ChunkedTerrain() {
    for(auto& row: chunks) {
        for(auto& cell: row) {
            glDeleteVertexArrays(1, &cell.vaoId);
            glDeleteBuffers(1, &cell.bufferId);
            glDeleteBuffers(1, &cell.eboId);
        }
    }

    glDeleteTextures(1, &textureId);
}

void ChunkedTerrain::draw(int chunkX, int chunkZ) {

    shader->use();

    shader->setInt("terrain", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_CW);

    if(drawAll) {
        for(auto& row: chunks)
            for(auto& cell: row) {
                glBindVertexArray(cell.vaoId);
                glDrawArrays(GL_TRIANGLES, 0, cell.vertexCount);
            }
    }
    else {
        int chunkRadius = 5; // number of chunks to draw in each direction

        for( int z = std::max(chunkZ - chunkRadius, 0); z < std::min(chunkZ + chunkRadius, chunksPerSide -1); z++) {
            for( int x = std::max(chunkX - chunkRadius, 0); x < std::min(chunkX + chunkRadius, chunksPerSide -1); x++ ) {
                auto& cell = chunks[z][x];

                glBindVertexArray(cell.vaoId);
                glDrawArrays(GL_TRIANGLES, 0, cell.vertexCount);
            }
        }
    }

    glDisable(GL_CULL_FACE);

    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ChunkedTerrain::UploadTexture() {
    GLsizei width = 128, height = 128, layerCount = rsc->textures.size(), mipLevelCount = 1;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGB5_A1, width, height, layerCount);

    std::vector<GLushort> tex(rsc->textures.size() * 128 * 128, 0);
    for(auto i=0; i < rsc->textures.size(); i++) {
        memcpy(&tex[i * 128 * 128], rsc->textures[i].data, rsc->textures[i].size);
    }

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layerCount,
                    GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, &tex[0]);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}


glm::vec2 rotateUV(glm::vec2 uv, float rotation) {
    auto pivot = glm::vec2{ .5, .5 };

    float cosa = cos(M_PI / 2.f * rotation);
    float sina = sin(M_PI / 2.f * rotation);

    uv -= pivot;
    return glm::vec2(
            cosa * uv.x - sina * uv.y,
            cosa * uv.y + sina * uv.x
    ) + pivot;
}

// get triangle normal
glm::vec3 calculateTriangleNormal( glm::vec3 A, glm::vec3 B ) {
    return glm::normalize( glm::cross(A, B) );
}

std::vector<TerrainVertex> ChunkedTerrain::GetRegion(int chunkX, int chunkZ, int valuesPerChunkRow) {

    int horizontalOffset = chunkX * chunksPerSide,
            verticalOffset = chunkZ * chunksPerSide;

    std::vector<TerrainVertex> vertices;


    for(auto z = verticalOffset; z < (verticalOffset + valuesPerChunkRow); z++) {
        for(auto x = horizontalOffset; x < (horizontalOffset + valuesPerChunkRow); x++) {
            int idx = z * 1024 + x;

            glm::vec2 coords[6] = {
                {0, 0},
                {1, 0},
                {1, 1},
                {0, 0},
                {1, 1},
                {0, 1}
            };

            int flags = map->bitflagMap[z * 1024 + x];

            float amount = 0;
            if((flags & 0x01) == 0x0001) amount += glm::radians(-90.f);
            if((flags & 0x10) == 0x0010) amount += glm::radians(-90.f);
            if((flags & 0x11) == 0x0011) amount += glm::radians(-90.f);

            uint rotation = (flags&OCARN2::BF_TEXTURE_DIRECTION | (flags&OCARN2::BF_REVERSE << 6) ) & 3u;

            for(auto& tex: coords) {
                tex = rotateUV(tex, rotation);
            }

            glm::vec3 v1 = {x, GetHeight(z * 1024 + x), z},
                v2 = {x + 1, GetHeight(z * 1024 + (x + 1)), z},
                v3 = {x + 1, GetHeight((z + 1) * 1024 + (x + 1)), z + 1};
            auto normalA = calculateTriangleNormal(v2 - v1, v3 - v1);

            // first triangle upper left
            // ccw winding
            vertices.emplace_back(TerrainVertex {
                    v1,
                    {coords[0].s, coords[0].t, map->textureMap[idx]},
                    normalA
            });
            vertices.emplace_back(TerrainVertex {
                    v3,
                    {coords[2].s, coords[2].t, map->textureMap[idx]},
                    normalA
            });
            vertices.emplace_back(TerrainVertex {
                    v2,
                    {coords[1].s, coords[1].t, map->textureMap[idx]},
                    normalA
            });


            v1 = {x, GetHeight(z * 1024 + x), z},
                v2 = {x + 1, GetHeight((z + 1) * 1024 + (x + 1)), z + 1},
                v3 = {x, GetHeight((z + 1) * 1024 + x), z + 1};
            auto normalB = calculateTriangleNormal(v2 - v1, v3 - v1);

            // second triangle, bottom right
            // ccw winding
            vertices.emplace_back(TerrainVertex {
                    v1,
                    {coords[3].s, coords[3].t, map->textureMap[idx]},
                    normalB
            });
            vertices.emplace_back(TerrainVertex {
                    v3,
                    {coords[5].s, coords[5].t, map->textureMap[idx]},
                    normalB
            });
            vertices.emplace_back(TerrainVertex {
                    v2,
                    {coords[4].s, coords[4].t, map->textureMap[idx]},
                    normalB
            });

            // now handle setting up spawn locations
            if(map->objectMap[z * 1024 + x] == 254) {
                map->objectMap[z * 1024 + x] = 255;

                spawns.emplace_back( x, GetHeight(x, z), z );
            }
        }
    }

    return vertices;
}

float ChunkedTerrain::GetHeight(int x, int z) {
    return GetHeight(z * 1024 + x);
}

float ChunkedTerrain::GetHeight(int idx) {
    return (float) map->heightMap[idx] / 4.f;
}

glm::vec3 ChunkedTerrain::GetRandomSpawnLocation() {
    if( spawns.empty() ) return glm::vec3 { 238, GetHeight(238, 278), 278 };

    return spawns[rand() % (spawns.size() - 1)];
}
