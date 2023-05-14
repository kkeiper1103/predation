//
// Created by kkeiper1103 on 12/20/22.
//

#ifndef PREDATION_SKYBOX_H
#define PREDATION_SKYBOX_H

#include <glad/glad.h>
#include "graphics/Shader.h"

class Skybox {
protected:
    GLuint vaoId{0};
    GLuint buffers[4] {0, 0, 0, 0};
    GLuint textureId{0};

    float windspeed = 0.5f;

    int width{256};
    int height{256};

    glm::vec3 rotation {0, 0, 0};
    glm::mat4 model {1.f};

    // for now, the shader needs to be public for all the classes that modify it
public:
    std::shared_ptr<Shader> shader = nullptr;

public:
    explicit Skybox(GLushort* texture);
    ~Skybox();

    void update(double dt);
    void draw();

protected:
    struct vertex_collection_t {
        std::vector<GLuint> indices;
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> uvs;
    };

    vertex_collection_t generateVertices();

    unsigned long numElements{0};


    //
    // Helper functions to pack and unpack a pixel
    GLushort packPixel(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
        return (r << 11) | (g << 6) | (b << 1) | a;
    }
    //
    void unpackPixel(GLushort pixel, unsigned int& r, unsigned int& g, unsigned int& b, unsigned int& a) {
        r = (pixel >> 11) & 0x1F;
        g = (pixel >> 6) & 0x1F;
        b = (pixel >> 1) & 0x1F;
        a = pixel & 0x1;
    }

    //
    std::vector<GLushort> upscaleNearestNeighbor(GLushort* original) {
        // The original image is 256x256
        const int originalWidth = 256;
        const int originalHeight = 256;

        // The upscaled image is 1024x1024
        const int upscaledWidth = 1024;
        const int upscaledHeight = 1024;

        // Create a vector to hold the upscaled pixel data
        std::vector<GLushort> upscaled(upscaledWidth * upscaledHeight);

        // For each pixel in the upscaled image
        // For each pixel in the upscaled image
        for (int y = 0; y < upscaledHeight; ++y) {
            for (int x = 0; x < upscaledWidth; ++x) {
                // Map the upscaled coordinates to the original coordinates
                float originalX = static_cast<float>(x) / upscaledWidth * originalWidth;
                float originalY = static_cast<float>(y) / upscaledHeight * originalHeight;

                // Calculate the coordinates of the 2x2 pixel neighborhood in the original image
                int x1 = static_cast<int>(originalX);
                int x2 = std::min(x1 + 1, originalWidth - 1);
                int y1 = static_cast<int>(originalY);
                int y2 = std::min(y1 + 1, originalHeight - 1);

                // Calculate the interpolation weights
                float dx = originalX - x1;
                float dy = originalY - y1;


                // Unpack the neighborhood pixels into color channels
                unsigned int r1, g1, b1, a1, r2, g2, b2, a2, r3, g3, b3, a3, r4, g4, b4, a4;
                unpackPixel(original[y1 * originalWidth + x1], r1, g1, b1, a1);
                unpackPixel(original[y1 * originalWidth + x2], r2, g2, b2, a2);
                unpackPixel(original[y2 * originalWidth + x1], r3, g3, b3, a3);
                unpackPixel(original[y2 * originalWidth + x2], r4, g4, b4, a4);

                // Perform bilinear interpolation on each color channel separately
                unsigned int r = static_cast<unsigned int>((1 - dx) * (1 - dy) * r1 + dx * (1 - dy) * r2 + (1 - dx) * dy * r3 + dx * dy * r4 + 0.5);
                unsigned int g = static_cast<unsigned int>((1 - dx) * (1 - dy) * g1 + dx * (1 - dy) * g2 + (1 - dx) * dy * g3 + dx * dy * g4 + 0.5);
                unsigned int b = static_cast<unsigned int>((1 - dx) * (1 - dy) * b1 + dx * (1 - dy) * b2 + (1 - dx) * dy * b3 + dx * dy * b4 + 0.5);
                unsigned int a = static_cast<unsigned int>((1 - dx) * (1 - dy) * a1 + dx * (1 - dy) * a2 + (1 - dx) * dy * a3 + dx * dy * a4 + 0.5);

                //
                upscaled[y * upscaledWidth + x] = packPixel(r, g, b, a);
            }
        }

        return upscaled;
    }
};


#endif //PREDATION_SKYBOX_H
