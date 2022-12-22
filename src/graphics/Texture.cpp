//
// Created by kkeiper1103 on 11/14/22.
//

#define STB_IMAGE_IMPLEMENTATION
#include "Texture.h"


Texture::Texture(const std::string &file, GLint internalFormat, GLenum format, GLenum type) : internalFormat(internalFormat), format(format), type(type) {
    data = stbi_load(file.c_str(), &width, &height, &channels, 4);
    if(!data) {
        LOG(ERROR) << "Unable to Load [" << file << "]";
    }

    glGenTextures(1, &id);

    upload();
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

Texture::Texture(unsigned char *data, int width, int height, GLint internalFormat, GLenum format, GLenum type) :
data(data), width(width), height(height), internalFormat(internalFormat), format(format), type(type) {
    glGenTextures(1, &id);

    upload();
}

void Texture::upload() {
    // upload it
    glBindTexture(GL_TEXTURE_2D, id);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // if a parameter is > 1.f, just repeat the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // same
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // this moves the pixels onto the gpu using the parameters defined
        glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, data);

        // without this call, textures will be blank because gl is expecting to find mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}
