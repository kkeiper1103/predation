//
// Created by kkeiper1103 on 12/18/22.
//

#ifndef PREDATION_CHUNKEDTERRAIN_H
#define PREDATION_CHUNKEDTERRAIN_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ocarn2.h>
#include "graphics/Shader.h"

#include "ITerrain.h"


class ChunkedTerrain : public ITerrain {
protected:
    // texture_2d_array variable
    GLuint textureId{0};
    std::vector<std::vector<TerrainChunk>> chunks;

    OCARN2::Map* map = nullptr;
    OCARN2::Rsc* rsc = nullptr;

    bool drawAll = false;

private:
    void UploadTexture();

    std::vector<TerrainVertex> GetRegion(int chunkX, int chunkZ, int valuesPerChunkRow);

    std::vector<glm::vec3> spawns;

public:

    ChunkedTerrain(OCARN2::Map* map, OCARN2::Rsc* rsc, int chunksPerSide);
    ~ChunkedTerrain();


    void draw(int chunkX, int chunkZ) override;


    float GetHeight(int x, int z) override;
    float GetHeight(int idx) override;

    glm::vec3 GetRandomSpawnLocation() override;
};


#endif //PREDATION_CHUNKEDTERRAIN_H
