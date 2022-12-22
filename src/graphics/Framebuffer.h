//
// Created by kkeiper1103 on 11/26/22.
//

#ifndef BREAKOUTGL_FRAMEBUFFER_H
#define BREAKOUTGL_FRAMEBUFFER_H

#include <glad/glad.h>
#include <functional>
#include <easyloggingpp/easylogging++.h>

/**
 * @class multisampled framebuffer to allow antialiasing
 */
class Framebuffer {
protected:
    friend class PostProcessor;

    GLuint id = 0;
    GLuint textureId = 0;
    GLuint renderBufferId = 0;

    // for transferring the sampled fbo to a regular one for rendering
    GLuint intermediateId = 0;
    GLuint intermediateTextureId = 0;
    GLuint intermediateRenderBufferId = 0;

private:
    // helper function to bind and also verify that the framebuffer is correct
    void bind();

public:
    Framebuffer();
    ~Framebuffer();

    void bindAndExecute(const std::function<void(const Framebuffer*)>& operations);
};


#endif //BREAKOUTGL_FRAMEBUFFER_H
