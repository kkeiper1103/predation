//
// Created by kkeiper1103 on 11/6/22.
//

#ifndef MODULETWO_MESH_H
#define MODULETWO_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "Vertex.h"

#include "Shader.h"
#include "Texture.h"


class Mesh {
public:
    typedef GLushort idx_t;

    std::shared_ptr<Shader> shader;

protected:
    GLuint vaoId = 0;
    GLuint vboId = 0;
    GLuint eboId = 0;

    std::vector<Vertex> vertices;
    std::vector<idx_t> indices;


    friend class Model;
    glm::vec3 minVertex = {FLT_MAX, FLT_MAX, FLT_MAX}, maxVertex = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    inline void calculateAABBPoints() {
        calculateMinPoint();
        calculateMaxPoint();
    }

    // this isn't calculating which vertex is furthest left, down, and back, but rather generating a new point from
    // the leftmost x, bottommost y, and back-most z;
    inline void calculateMinPoint() {
        for(auto& v: vertices) {
            if(v.position.x < minVertex.x) minVertex.x = v.position.x;
            if(v.position.y < minVertex.y) minVertex.y = v.position.y;
            // if(v.position.z < minVertex.z) minVertex.z = v.position.z;
        }
    }

    // this creates a point from the rightmost x, top-most y, and front-most z. combining both min and max vertices, we can create an AABBbox for frustum culling
    inline void calculateMaxPoint() {
        for(auto& v: vertices) {
            if(v.position.x > maxVertex.x) maxVertex.x = v.position.x;
            if(v.position.y > maxVertex.y) maxVertex.y = v.position.y;
            // if(v.position.z > maxVertex.z) maxVertex.z = v.position.z;
        }
    }

public:
    GLenum drawMode = GL_TRIANGLES; // this can be changed publicly to manage whether triangles, strips, fans, lines, etc are drawn

    // face culling settings
    bool cullFaces = false;
    GLenum cullFace = GL_FRONT;
    GLenum frontFace = GL_CCW;

public:
    explicit Mesh(std::vector<Vertex> vertices = {}, std::vector<GLushort> indices = {});
    ~Mesh();

    void update( double dt );
    void draw();

    void setData(std::vector<Vertex> vertices, std::vector<idx_t> indices);
};


#endif //MODULETWO_MESH_H
