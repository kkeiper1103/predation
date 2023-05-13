//
// Created by kkeiper1103 on 5/12/2023.
//

#include "RandomTerrain.h"

RandomTerrain::RandomTerrain(OCARN2::Map *map, OCARN2::Rsc *rsc, int chunksPerSide, unsigned int seed) : map(map),
rsc(rsc), chunksPerSide(chunksPerSide), seed(seed) {

    elevation.reseed(seed);
    temperature.reseed(seed);

    // create and set state
    glGenVertexArrays(1, &vaoId); glBindVertexArray(vaoId); glBindVertexArray(0);
    glGenTextures(1, &textureId); glBindTexture(GL_TEXTURE_2D_ARRAY, textureId); glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glGenBuffers(NUM_BUFFERS, bufferIds);
        for(auto i=0; i < NUM_BUFFERS; i++) { glBindBuffer( GL_ARRAY_BUFFER, bufferIds[i] ); }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    // generate perlin heights
    // @todo introduce multithreading to generate chunks
    heightmap.reserve(columns * rows);

    for(auto z=0; z < rows; z++) {
        for(auto x=0; x < columns; x++) {
            auto idx = z * columns + x;

            heightmap[idx] = maxHeight * elevation.octave2D_01(x * 0.01, z * 0.01, 4);
        }
    }

    // upload textures
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB5_A1, 128, 128, rsc->numTextures);

    // load each texture into a layer on the 2d array
    for(auto i=0; i < rsc->textures.size(); i++)
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 128, 128, 1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, rsc->textures[i].data);

    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // generate shader
    shader = Shader::FromFiles("resources/shaders/ChunkedTerrain.vert", "resources/shaders/ChunkedTerrain.frag");

    // texture coords for the triangles
    glm::vec2 coords[6] = {
            {0, 0},
            {1, 0},
            {1, 1},
            {0, 0},
            {1, 1},
            {0, 1}
    };

    // generate mesh
    std::vector<TerrainVertex> vertices;
    for(size_t z = 0; z < rows; z++) {
        for(size_t x = 0; x < columns; x++) {
            auto idx = (z * columns + x);

            glm::vec3 v1 = {  (x),      GetHeight(z * columns + x),                   (z) },
                    v2 = {  (x + 1),  GetHeight(z * columns + (x + 1)),          (z) },
                    v3 = {  (x + 1),  GetHeight((z + 1) * columns + (x + 1)),    (z + 1) },

            // second face
            v4 = {  (x),      GetHeight(z * columns + x),                   (z) },
                    v5 = {  (x + 1),  GetHeight((z + 1) * columns + (x + 1)),    (z + 1) },
                    v6 = {  (x),      GetHeight((z + 1) * columns + x),             (z + 1) };


            auto normalA = calculateTriangleNormal(v2 - v1, v3 - v1);
            auto normalB = calculateTriangleNormal(v5 - v4, v6 - v4);

            // first triangle upper left
            // ccw winding
            vertices.emplace_back(TerrainVertex {
                    v1,
                    {coords[0].x, coords[0].y, 0},
                    normalA
            });
            vertices.emplace_back(TerrainVertex {
                    v2,
                    {coords[1].x, coords[1].y, 0},
                    normalA
            });
            vertices.emplace_back(TerrainVertex {
                    v3,
                    {coords[2].x, coords[2].y, 0},
                    normalA
            });

            vertices.emplace_back(TerrainVertex {
                    v4,
                    {coords[3].x, coords[3].y, 0},
                    normalB
            });
            vertices.emplace_back(TerrainVertex {
                    v5,
                    {coords[4].x, coords[4].y, 0},
                    normalB
            });
            vertices.emplace_back(TerrainVertex {
                    v6,
                    {coords[5].x, coords[5].y, 0},
                    normalB
            });
        }
    }
}

RandomTerrain::~RandomTerrain() {
    glDeleteVertexArrays(1, &vaoId);
    glDeleteTextures(1, &textureId);
    glDeleteBuffers(NUM_BUFFERS, bufferIds);
}

void RandomTerrain::draw(int chunkX, int chunkZ) {

}

float RandomTerrain::GetHeight(int x, int z) {
    return GetHeight(z * columns + x);
}

float RandomTerrain::GetHeight(int idx) {
    return heightmap[idx];
}

glm::vec3 RandomTerrain::GetRandomSpawnLocation() {
    int x = 100, z = 100;

    return glm::vec3(x, GetHeight(x, z), z);
}
