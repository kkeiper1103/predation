//
// Created by kkeiper1103 on 12/22/22.
//

#ifndef PREDATION_ENTITYMESH_H
#define PREDATION_ENTITYMESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ocarn2.h"

#include <map>
#include <memory>

#include "graphics/Shader.h"
#include "glm/ext/matrix_transform.hpp"

class EntityMesh {
friend class Hunt;
protected:
    GLuint vaoId{0};
    GLuint vboId{0};
    GLuint textures[2] {0, 0};
    int vertexCount{0};


public:
    static std::shared_ptr<Shader> _shader;
    std::shared_ptr<Shader> shader;
    bool ready = false;

public:
    explicit EntityMesh(const OCARN2::Mesh *data);
    ~EntityMesh();

    void update(double dt) {}

    void drawAt(float x, float y, float z) {
        shader->use();
        shader->setInt("diffuse", 0);
        shader->setInt("specular", 1);

        glm::mat4 model(1.f);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(.01, .01, .01));
        shader->setMat4("model", model);

        // diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        // normal map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void draw() {
        shader->use();
        shader->setInt("diffuse", 0);
        shader->setInt("specular", 1);

        // diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        // normal map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        glBindVertexArray(vaoId);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};


#endif //PREDATION_ENTITYMESH_H
