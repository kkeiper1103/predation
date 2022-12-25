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

struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 texture; // <s, t, layerid>
    glm::vec3 normal;
};

struct TerrainChunk {
    GLuint vaoId{0};
    GLuint bufferId{0};
    GLuint eboId{0};

    std::vector<TerrainVertex> data;
    int vertexCount = 0;
    int indexCount = 0;

    int numStrips{0};
    int numVertsPerStrip{0};
};

class ChunkedTerrain {
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

public:
    std::shared_ptr<Shader> shader;
    int chunksPerSide;

    ChunkedTerrain(OCARN2::Map* map, OCARN2::Rsc* rsc, int chunksPerSide);
    ~ChunkedTerrain();


    void draw(int chunkX, int chunkZ);


    float GetHeight(int x, int z);
    float GetHeight(int idx);
};


#endif //PREDATION_CHUNKEDTERRAIN_H
