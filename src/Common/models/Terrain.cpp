//
// Created by kkeiper1103 on 5/28/2023.
//

#include "Terrain.h"

Chunk::Chunk() {
    glCreateVertexArrays(1, &vaoId);
    glCreateBuffers(1, &bufferId);
}

Chunk::~Chunk() {
    glDeleteBuffers(1, &bufferId);
    glDeleteVertexArrays(1, &vaoId);
}

void Chunk::Update(float dt) {

}

void Chunk::Render() {
    glBindVertexArray(vaoId);
    glDrawArrays(GL_TRIANGLES, 0, data.size() / 3);
}

Terrain::Terrain(OCARN2::Map *map, OCARN2::Rsc *resources) : map(map), rsc(resources) {
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &textureId);

    // upload texture to gpu
    GLsizei width = 128, height = 128, layerCount = rsc->textures.size(), mipLevelCount = 1;
    glTextureStorage3D(textureId, mipLevelCount, GL_RGB5_A1, width, height, layerCount);

    // load each texture into a layer on the 2d array
    for(auto i=0; i < rsc->textures.size(); i++)
        glTextureSubImage3D(textureId, 0, 0, 0, i, width, height, 1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, rsc->textures[i].data);

    glTextureParameteri(textureId,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(textureId,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(textureId,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureId,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);


    // pre-allocate the memory by creating empty chunks
    chunks.resize(chunksPerSide);
    for(int z=0; z < chunksPerSide; z++) {
        chunks[z].resize(chunksPerSide, Chunk::Ptr {new Chunk()});
    }

    // build chunks
    // now, fill the chunks
    for(int z=0; z < chunksPerSide; z++) {
        for(int x = 0; x < chunksPerSide; x++) {
            auto& chunk = chunks[z][x];

            auto data = GetRegion(x, z, mapSize / chunksPerSide);
            chunk->data = data;

            glNamedBufferData(chunk->bufferId, sizeof(TerrainVertex) * data.size(), &data[0], GL_STATIC_DRAW);
            glVertexArrayVertexBuffer(chunk->vaoId, 0, chunk->bufferId, 0, sizeof(TerrainVertex));

            glEnableVertexArrayAttrib(chunk->vaoId, 0);
            glVertexArrayAttribFormat(chunk->vaoId, 0, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, position));
            glVertexArrayAttribBinding(chunk->vaoId, 0, 0);

            glEnableVertexArrayAttrib(chunk->vaoId, 1);
            glVertexArrayAttribFormat(chunk->vaoId, 1, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, texture));
            glVertexArrayAttribBinding(chunk->vaoId, 1, 0);

            glEnableVertexArrayAttrib(chunk->vaoId, 2);
            glVertexArrayAttribFormat(chunk->vaoId, 2, 3, GL_FLOAT, GL_FALSE, offsetof(TerrainVertex, normal));
            glVertexArrayAttribBinding(chunk->vaoId, 2, 0);
        }
    }
}

Terrain::~Terrain() {
    glDeleteTextures(1, &textureId);
}

void Terrain::Update(float delta) {
    for(auto& row: chunks) {
        for(auto& chunk: row) chunk->Update(delta);
    }
}

void Terrain::Render() {
    glBindTextures(0, 1, &textureId);

    // @todo draw chunk-at-position instead of all-chunks
    for(auto& row: chunks) {
        for(auto& chunk: row) chunk->Render();
    }
}




glm::vec2 rotateUV(glm::vec2 uv, float rotation) {
    float mid = 0.5;

    float cos_ = cos(M_PI / 2. * rotation);
    float sin_ = sin(M_PI / 2. * rotation);

    return {
            cos_ * (uv.x - mid) + sin_ * (uv.y - mid) + mid,
            cos_ * (uv.y - mid) - sin_ * (uv.x - mid) + mid
    };
}


inline
glm::vec3 calculateTriangleNormal( glm::vec3 A, glm::vec3 B ) {
    return glm::normalize( glm::cross(A, B) );
}

//
#define BitVal(data,y) ( (data>>y) & 1)      /** Return Data.Y value   **/

std::vector<TerrainVertex> Terrain::GetRegion(int chunkX, int chunkZ, int valuesPerChunkRow) {

    int horizontalOffset = chunkX * chunksPerSide,
            verticalOffset = chunkZ * chunksPerSide;

    std::vector<TerrainVertex> vertices;

    const int size = 1024;

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

            bool reversed = BitVal(flags, 4) != 0;
            int tDirection = flags & 3u;

            int rotation = tDirection | (reversed << 6);

            for(auto& tex: coords) {
                tex = rotateUV(tex, rotation);
            }

            glm::vec3 v1 = {  (x),      GetHeight(z * size + x),                   (z) },
                    v2 = {  (x + 1),  GetHeight(z * size + (x + 1)),          (z) },
                    v3 = {  (x + 1),  GetHeight((z + 1) * size + (x + 1)),    (z + 1) },

            // second face
            v4 = {  (x),      GetHeight(z * size + x),                   (z) },
                    v5 = {  (x + 1),  GetHeight((z + 1) * size + (x + 1)),    (z + 1) },
                    v6 = {  (x),      GetHeight((z + 1) * size + x),             (z + 1) };


            auto normalA = calculateTriangleNormal(v2 - v1, v3 - v1);
            auto normalB = calculateTriangleNormal(v5 - v4, v6 - v4);

            // first triangle upper left
            // ccw winding
            vertices.emplace_back(TerrainVertex {
                    v1,
                    {coords[0].x, coords[0].y, map->textureMap[idx]},
                    normalA
            });
            vertices.emplace_back(TerrainVertex {
                    v2,
                    {coords[1].x, coords[1].y, map->textureMap[idx]},
                    normalA
            });
            vertices.emplace_back(TerrainVertex {
                    v3,
                    {coords[2].x, coords[2].y, map->textureMap[idx]},
                    normalA
            });

            vertices.emplace_back(TerrainVertex {
                    v4,
                    {coords[3].x, coords[3].y, map->textureMap[idx]},
                    normalB
            });
            vertices.emplace_back(TerrainVertex {
                    v5,
                    {coords[4].x, coords[4].y, map->textureMap[idx]},
                    normalB
            });
            vertices.emplace_back(TerrainVertex {
                    v6,
                    {coords[5].x, coords[5].y, map->textureMap[idx]},
                    normalB
            });

            // now handle setting up spawn locations
            if(map->objectMap[z * 1024 + x] == 254) {
                map->objectMap[z * 1024 + x] = 255;
            }
        }
    }

    return vertices;
}

float Terrain::GetHeight(int x, int z) {
    return GetHeight(z * 1024 + x);
}

float Terrain::GetHeight(int idx) {
    return (float) map->heightMap[idx] / 4.f;
}