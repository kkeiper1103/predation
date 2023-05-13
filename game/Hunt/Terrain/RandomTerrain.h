//
// Created by kkeiper1103 on 5/12/2023.
//

#ifndef PREDATION_RANDOMTERRAIN_H
#define PREDATION_RANDOMTERRAIN_H

#include <memory>
#include <vector>
#include <random>

#include <glad/glad.h>
#include <siv/PerlinNoise.hpp>
#include <ocarn2.h>
#include "graphics/Shader.h"

#include "ITerrain.h"

class RandomTerrain : public ITerrain {
public:
    RandomTerrain(OCARN2::Map* map, OCARN2::Rsc* rsc, int chunksPerSide, unsigned int seed = 0);
    ~RandomTerrain();

    inline void DrawTerrain() {
        shader->use();

        glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(vaoId);

        glDrawArrays(GL_TRIANGLES, 0, columns * rows * 3);

        glBindVertexArray(0);
    }

    void draw(int chunkX, int chunkZ) override;

    float GetHeight(int x, int z) override;

    float GetHeight(int idx) override;

    glm::vec3 GetRandomSpawnLocation() override;


protected:
    OCARN2::Map* map = nullptr;
    OCARN2::Rsc* rsc = nullptr;
    std::shared_ptr<Shader> shader;

    unsigned int seed = 0;
    int chunksPerSide = 4;


    size_t columns {1024}, rows {1024};
    unsigned short maxHeight = 127;

    siv::PerlinNoise elevation;
    siv::PerlinNoise temperature;
    std::vector<GLfloat> heightmap;

    // gl settings
    constexpr static int NUM_BUFFERS = 3;
    GLuint vaoId {0}, textureId {0}, bufferIds[NUM_BUFFERS] {0,0,0};
};


#endif //PREDATION_RANDOMTERRAIN_H
