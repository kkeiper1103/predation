//
// Created by kkeiper1103 on 12/27/22.
//

#include "SmoothedTerrain.h"

#include "Hunt/Hunt.h"

SmoothedTerrain::SmoothedTerrain(Hunt *parent, OCARN2::Map *map, OCARN2::Rsc *rsc, int factor) : parent(parent), map(map), rsc(rsc), factor(factor) {
    glGenVertexArrays(1, &vaoId);
    glGenTextures(1, &textureId);
    glGenBuffers(1, &vboId);

    heightmap.resize( map->heightMap.size() * factor, -1 );

    for(int i=0; i < map->heightMap.size(); i++) {
        int startingOffset = i * factor;

        heightmap[startingOffset] = (float) map->heightMap[i];

        for(int j=1; j <= factor; j++) {
            heightmap[startingOffset + j] = (float) (map->heightMap[i] + map->heightMap[i + 1]) / (float) factor;
        }
    }

    // build vertices


    shader = Shader::FromFiles("resources/shaders/ChunkedTerrain.vert", "resources/shaders/ChunkedTerrain.frag");
}

SmoothedTerrain::~SmoothedTerrain() {
    glDeleteBuffers(1, &vboId);
    glDeleteTextures(1, &textureId);
    glDeleteVertexArrays(1, &vaoId);
}
