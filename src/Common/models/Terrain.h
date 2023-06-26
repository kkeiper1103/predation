//
// Created by kkeiper1103 on 5/28/2023.
//

#ifndef PREDATION_TERRAIN_H
#define PREDATION_TERRAIN_H

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ocarn2.h>

struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 texture; // <s, t, layerid>
    glm::vec3 normal;
};

/**
 *
 */
class Chunk {
public:
    using Ptr = std::shared_ptr<Chunk>;

public:
    Chunk();
    ~Chunk();

    void Update(float dt);
    void Render();

protected:
    friend class Terrain;

    GLuint vaoId {0};
    GLuint bufferId {0};

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> texCoords;

    std::vector<TerrainVertex> data;
};

/**
 *
 */
class Terrain {
public:
    Terrain(OCARN2::Map* map, OCARN2::Rsc* resources);
    virtual ~Terrain();

    void Update(float delta);
    void Render();

protected:
    OCARN2::Map* map;
    OCARN2::Rsc* rsc;

    int mapSize = 1024;
    int chunksPerSide = 16;
    std::vector<std::vector<Chunk::Ptr>> chunks;

    GLuint textureId {0};



    std::vector<TerrainVertex> GetRegion(int chunkX, int chunkZ, int valuesPerChunkRow);
    float GetHeight(int x, int z);
    float GetHeight(int idx);
};

#endif //PREDATION_TERRAIN_H
