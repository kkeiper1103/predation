//
// Created by kkeiper1103 on 12/27/22.
//

#ifndef PREDATION_SMOOTHEDTERRAIN_H
#define PREDATION_SMOOTHEDTERRAIN_H

#include <glad/glad.h>
#include <stb/stb_image_write.h>
#include <glm/glm.hpp>
#include <ocarn2.h>
#include "graphics/Shader.h"
#include "ITerrain.h"

class Hunt;



class SmoothedTerrain : public ITerrain {
protected:
    GLuint vaoId {0};
    GLuint textureId {0};
    GLuint vboId {0};

    // need a place to store the heightmap, since we're subdividing
    std::vector<float> heightmap;

    Hunt* parent {nullptr};
    OCARN2::Map* map {nullptr};
    OCARN2::Rsc* rsc {nullptr};

    int factor = 4;

    std::shared_ptr<Shader> shader;

public:
    SmoothedTerrain(Hunt* parent, OCARN2::Map* map, OCARN2::Rsc* rsc, int factor = 4);
    ~SmoothedTerrain();

    inline void draw() {
        shader->use();

        shader->setInt("terrain", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_CW);


        glDrawArrays(GL_TRIANGLES, 0, heightmap.size()); // number of heights should always equal the number of verts


        glDisable(GL_CULL_FACE);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
};


#endif //PREDATION_SMOOTHEDTERRAIN_H
