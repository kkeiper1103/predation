//
// Created by kkeiper1103 on 5/12/2023.
//

#ifndef PREDATION_ITERRAIN_H
#define PREDATION_ITERRAIN_H

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 texture; // <s, t, layerid>
    glm::vec3 normal;
};

struct TerrainChunk {
    GLuint vaoId{0};
    GLuint bufferId{0};
    GLuint eboId{0};

    int vertexCount = 0;
    std::vector<TerrainVertex> data;
};

/**
 *
 */
class ITerrain {
public:

    virtual void draw(int chunkX, int chunkZ)=0;

    virtual float GetHeight(int x, int z)=0;
    virtual float GetHeight(int idx) = 0;

    virtual glm::vec3 GetRandomSpawnLocation() = 0;


    std::shared_ptr<Shader> shader;
    int chunksPerSide {1};
};


// get triangle normal
inline
glm::vec3 calculateTriangleNormal( glm::vec3 A, glm::vec3 B ) {
    return glm::normalize( glm::cross(A, B) );
}

#endif //PREDATION_ITERRAIN_H
