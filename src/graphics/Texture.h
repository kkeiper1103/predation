//
// Created by kkeiper1103 on 11/14/22.
//

#ifndef PROJECTONE_TEXTURE_H
#define PROJECTONE_TEXTURE_H


#include <string>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <easyloggingpp/easylogging++.h>

class Texture {
protected:
    GLuint id;

    int width, height, channels;
    unsigned char* data = nullptr;

    //
    GLint internalFormat = GL_RGBA;
    GLenum format = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;

    //
    int level = 0;
    int border = 0;

private:
    void upload();

public:
    Texture(unsigned char* data, int width, int height, GLint internalFormat = GL_RGBA, GLenum sourceFormat = GL_RGBA, GLenum sourceDataType = GL_UNSIGNED_BYTE);
    explicit Texture(const std::string& file, GLint internalFormat = GL_RGBA, GLenum sourceFormat = GL_RGBA, GLenum sourceDataType = GL_UNSIGNED_BYTE);
    ~Texture();


    inline void apply(int slotOffset = 0) const {
        glActiveTexture(GL_TEXTURE0 + slotOffset);
        glBindTexture(GL_TEXTURE_2D, id);
    }
};


#endif //PROJECTONE_TEXTURE_H
