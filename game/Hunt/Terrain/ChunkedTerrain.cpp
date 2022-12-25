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

    std::vector<GLushort> indices;
    // should the indices be the same for all chunks, since they reset?
    int width = 1024 / chunksPerSide,
        height = width;

    for(unsigned int i = 0; i < height-1; i++) {
        for(unsigned int j = 0; j < width; j++) {
            for(unsigned int k = 0; k < 2; k++) {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    // now, fill the chunks
    for(int z=0; z < chunksPerSide; z++) {
        for(int x = 0; x < chunksPerSide; x++) {
            auto& chunk = chunks[z][x];

            auto data = GetRegion(x, z, 1024 / chunksPerSide);
            chunk.data = data;
            chunk.vertexCount = data.size();
            chunk.indexCount = indices.size();
            chunk.numVertsPerStrip = width * 2;
            chunk.numStrips = height - 1;

            glGenVertexArrays(1, &chunk.vaoId);
            glBindVertexArray(chunk.vaoId);
            {
                glGenBuffers(1, &chunk.bufferId);
                glBindBuffer(GL_ARRAY_BUFFER, chunk.bufferId);
                glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * data.size(), &data[0], GL_STATIC_DRAW);

                glGenBuffers(1, &chunk.eboId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.eboId);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);

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

    if(drawAll) {
        for(auto& row: chunks)
            for(auto& cell: row) {
                glBindVertexArray(cell.vaoId);
                // glDrawArrays(GL_LINES, 0, cell.vertexCount);

                for(auto strip = 0; strip < cell.numStrips; ++strip)
                    glDrawElements(GL_TRIANGLE_STRIP, cell.numVertsPerStrip, GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * cell.numVertsPerStrip * strip));
            }
    }
    else {
        int chunkRadius = 2; // number of chunks to draw in each direction
        for( int z = std::max(chunkZ - chunkRadius, 0); z < std::min(chunkZ + chunkRadius, chunksPerSide -1); z++) {
            for( int x = std::max(chunkX - chunkRadius, 0); x < std::min(chunkX + chunkRadius, chunksPerSide -1); x++ ) {

                auto& cell = chunks[z][x];

                glBindVertexArray(cell.vaoId);
                for(auto strip = 0; strip < cell.numStrips; ++strip)
                    glDrawElements(GL_TRIANGLE_STRIP, cell.numVertsPerStrip, GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * cell.numVertsPerStrip * strip));
            }
        }
    }


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

std::vector<TerrainVertex> ChunkedTerrain::GetRegion(int chunkX, int chunkZ, int valuesPerChunkRow) {

    int horizontalOffset = chunkX * chunksPerSide,
        verticalOffset = chunkZ * chunksPerSide;

    std::vector<TerrainVertex> vertices;

    for(auto z = verticalOffset; z < verticalOffset + valuesPerChunkRow; z++) {
        for(auto x = horizontalOffset; x < horizontalOffset + valuesPerChunkRow; x++) {
            // @todo make this configurable
            int idx = z * 1024 + x;

            // not sure?
            // original carn2 archive source says the Y is scaled by 64 and the x/z are scaled by 256. seems HUGE
            vertices.emplace_back(TerrainVertex {
                {x, GetHeight(idx), z},
                {.5, .5, map->textureMap[idx]},
                {0, 0, 0}
            });
        }
    }

    return vertices;
}

float ChunkedTerrain::GetHeight(int x, int z) {
    return GetHeight(z * 1024 + x);
}

float ChunkedTerrain::GetHeight(int idx) {
    return (float) map->heightMap[idx] / 2.f;
}
