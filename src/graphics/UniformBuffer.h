//
// Created by kkeiper1103 on 5/7/2023.
//

#ifndef TILEMAP_UNIFORMBUFFER_H
#define TILEMAP_UNIFORMBUFFER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

struct UniformBlockData {
    glm::mat4 projection {1.f};
    glm::mat4 view {1.f};
};

template <class T>
struct UniformBuffer {
    const GLuint id {0};


    static UniformBuffer create() {
        GLuint id; glGenBuffers(1, &id);

        return UniformBuffer(id);
    }

private:
    explicit UniformBuffer(GLuint id) : id{id} {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
public:

    ~UniformBuffer() {
        glDeleteBuffers(1, &id);
    }

    void setData(T* data) {
        glBindBuffer(GL_UNIFORM_BUFFER, id);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void bindToBlock(int index) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, id);
    }
};


#endif //TILEMAP_UNIFORMBUFFER_H
