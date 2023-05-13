#pragma once

#include <unordered_map>
#include <glad/glad.h>

namespace gl_compat {
    inline std::unordered_map<GLuint, GLenum> texture_name_target_container;
    inline GLuint last_bound_texture = 0;

    inline void bindTexture(GLenum target, GLuint texture) {
        if(texture == last_bound_texture) return; // if it's the same texture, don't send gpu commands

        last_bound_texture = texture;
        glBindTexture(target, texture);
    }
}

#ifndef glCreateTextures
inline void glCreateTextures(GLenum target, GLsizei num, GLuint* names) {
    using namespace gl_compat;
    // @todo add DSA via an extension

    glGenTextures(num, names);
    for(size_t i=0; i < num; i++) {
        bindTexture(target, names[i]);
        gl_compat::texture_name_target_container[names[i]] = target;
    }
    glBindTexture(target, 0);
}
#endif


#ifndef glTextureImage2D
inline
void glTextureImage2D(GLuint name,
                      GLint level,
                      GLint internalformat,
                      GLsizei width,
                      GLsizei height,
                      GLint border,
                      GLenum format,
                      GLenum type,
                      const void * data) {
    using namespace gl_compat;

    // @todo add DSA via an extension
    GLenum target = gl_compat::texture_name_target_container[name];

    bindTexture(target, name);

    glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}
#endif


#ifndef glTextureImage3D
inline
void glTextureImage3D(	GLuint name,
                          GLint level,
                          GLint internalformat,
                          GLsizei width,
                          GLsizei height,
                          GLsizei depth,
                          GLint border,
                          GLenum format,
                          GLenum type,
                          const void * data) {
    using namespace gl_compat;

    // @todo add DSA via an extension
    GLenum target = gl_compat::texture_name_target_container[name];

    bindTexture(target, name);

    glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, data);
}
#endif


#ifndef glTextureSubImage2D
inline void glTextureSubImage2D(GLuint texture,
                                GLint level,
                                GLint xoffset,
                                GLint yoffset,
                                GLsizei width,
                                GLsizei height,
                                GLenum format,
                                GLenum type,
                                const void *pixels) {
    using namespace gl_compat;
    GLenum target = gl_compat::texture_name_target_container[texture];

    bindTexture(target, texture);
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
};
#endif


#ifndef glBindTextures
inline void glBindTextures(GLuint first, GLsizei count, const GLuint* textures) {
    using namespace gl_compat;

    for (GLsizei i = 0; i < count; i++) {
        GLuint texture;
        if (textures == NULL) {
            texture = 0;
        } else {
            texture = textures[i];
        }
        glActiveTexture(GL_TEXTURE0 + first + i);
        if (texture != 0) {
            GLenum target = gl_compat::texture_name_target_container[texture];
            bindTexture(target, textures[i]);
        }
    }
}
#endif

#ifndef glBindTextureUnit
inline void glBindTextureUnit(GLuint unit, GLuint texture) {
    using namespace gl_compat;

    GLenum target = gl_compat::texture_name_target_container[texture];

    glActiveTexture(GL_TEXTURE0 + unit);
    bindTexture(target, texture);
}
#endif

#ifndef glTextureParameteri
inline void glTextureParameteri(GLuint texture, GLenum name, GLint value) {
    using namespace gl_compat;

    bindTexture(texture_name_target_container[texture], texture);

    glTexParameteri(texture_name_target_container[texture], name, value);
}
#endif