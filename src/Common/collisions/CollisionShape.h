//
// Created by kkeiper1103 on 1/8/23.
//

#ifndef PREDATION_COLLISIONSHAPE_H
#define PREDATION_COLLISIONSHAPE_H

#include <glad/glad.h>
#include "graphics/Shader.h"

class CollisionShape {
public:
    CollisionShape() {
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vbo);


        glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(float));
        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, 0, 0);

        // should I switch to Mesh::Ptr for vertices?

        if( shader == nullptr ) {
            shader = Shader::FromFiles("resources/shaders/Bounds.vert", "resources/shaders/Bounds.frag");
        }
    }
    virtual ~CollisionShape() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    virtual bool intersects(CollisionShape* other) = 0;



    static inline Shader::Ptr shader { nullptr };

    GLuint vao {0};
    GLuint vbo {0};
    glm::vec3 lineColor {1.f, .2f, .2f};
    bool draw = true;

    CollisionShape& render() {
        shader->use();
        shader->setVec3("lineColor", lineColor);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36); // bounding box has 36 vertices @todo switch to rendering in child class
        glBindVertexArray(0);

        return *this;
    }
};

#endif //PREDATION_COLLISIONSHAPE_H
