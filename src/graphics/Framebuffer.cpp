//
// Created by kkeiper1103 on 11/26/22.
//

#include "Framebuffer.h"

#include "Application.h"

Framebuffer::Framebuffer() {

    auto const game = &app()->config;

    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    {
        /**
         * Generate the texture to be used to store results of blitting to framebuffer
         */
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureId);
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, game->width, game->height, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureId, 0);

        /**
         * Create Renderbuffer object for direct reading
         */
        glGenRenderbuffers(1, &renderBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
        {
            // @todo allow configuring of msaa passes instead of hardcoded 4
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, game->width, game->height);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR) << "The MSAA Framebuffer is not Complete!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // now, set up the intermediate fbo
    glGenFramebuffers(1, &intermediateId);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateId);
    {
        glGenTextures(1, &intermediateTextureId);
        glBindTexture(GL_TEXTURE_2D, intermediateTextureId);
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, game->width, game->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, intermediateTextureId, 0);

        //
        glGenRenderbuffers(1, &intermediateRenderBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, intermediateRenderBufferId);
        {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, game->width, game->height);
        }
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR) << "The Intermediate Framebuffer is not Complete!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &textureId);
    glDeleteRenderbuffers(1, &renderBufferId);

    glDeleteFramebuffers(1, &intermediateId);
    glDeleteTextures(1, &intermediateTextureId);
    glDeleteRenderbuffers(1, &renderBufferId);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR) << "Bound Framebuffer is not Complete!";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Framebuffer::bindAndExecute(const std::function<void(const Framebuffer *)> &operations) {
    bind();

    operations(this);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
