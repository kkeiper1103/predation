#pragma once

#include <glad/glad.h>
#include <unordered_map>

#include "gl_compat_textures.h"

namespace gl_compat {
    inline std::unordered_map<GLuint, GLenum> buffer_name_target_container;
}


#ifndef glCreateBuffers
inline void glCreateBuffers(GLsizei num, GLuint* names, GLenum target = GL_ARRAY_BUFFER) {


    glGenBuffers(num, names);
    for(size_t i=0; i < num; i++)
        glBindBuffer(target, names[i]);

    glBindBuffer(target, 0);
}
#endif





#ifndef glCreateVertexArrays
inline void glCreateVertexArrays(GLsizei num, GLuint* names) {


    glGenVertexArrays(num, names);
    for(size_t i=0; i < num; i++)
        glBindVertexArray(names[i]);

    glBindVertexArray(0);
}
#endif