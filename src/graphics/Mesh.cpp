//
// Created by kkeiper1103 on 11/6/22.
//

#include "Mesh.h"

/**
 * Generate the gl resources for the VAO, VBO, and IBO/EBO, then fill the buffers with the passed-in data
 *
 * @param vertices
 * @param indices
 */
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLushort> indices) : vertices(std::move(vertices)), indices(std::move(indices)) {
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glGenBuffers(1, &eboId);

    setData(this->vertices, this->indices);

    shader = Shader::FromFiles("resources/shaders/Mesh.vert", "resources/shaders/Mesh.frag");
}

/**
 * delete the gl resources when the object goes out of scope
 */
Mesh::~Mesh() {
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &eboId);

    glDeleteVertexArrays(1, &vaoId);
}

/**
 * Not used yet. intended for an update loop, but not sure if a mesh really needs updated. Possibly for an animated mesh?
 * @param dt
 */
void Mesh::update(double dt) {

}

void Mesh::draw() {

    // bind the shader and then assign each material associated with the mesh to the shader's texture unit
    // materials[0] gets bound to texture0, materials[1] -> texture1, materials[2] -> texture2
    shader->use();

    // handle culling if enabled
    if(cullFaces) {
        glEnable(GL_CULL_FACE);
        glFrontFace(frontFace);
        glCullFace(cullFace);
    }
    else {
        glDisable(GL_CULL_FACE);
    }

    // bind the mesh's vertex array object
    glBindVertexArray(vaoId);
    { // these braces aren't necessary, but I find that I like the conceptual information they provide, since they show that glDrawElements is supposed to happen only while the vao is bound
        glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, 0);
    }
    glBindVertexArray(0); // unbind the vao so contamination doesn't happen


    // turn off face culling
    glDisable(GL_CULL_FACE);
}

void Mesh::setData(std::vector<Vertex> vertices, std::vector<idx_t> indices) {
    this->vertices = std::move(vertices);
    this->indices = std::move(indices);

    glBindVertexArray(vaoId);
    { // see note in render method

        /**
         * tell gl that the buffer it should write to is the vbo
         * one thing of note is that we have to use the fully qualified member variable, not the local variable specifically because we called std::move on it earlier
         * vertices is empty; this->vertices now holds what vertices used to
         */
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(), &this->vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx_t) * this->indices.size(), &this->indices[0], GL_STATIC_DRAW);

        /**
         * This is a neat little trick to improve extensibility
         * offsetof is a macro that will get the memory address offset of the named property of the first parameter
         * so offsetof(Vertex, position) will return a pointer to where position is stored within Vertex
         * This means we can move the properties within the struct around and this code won't break
         *
         * It specifically removes the need for mental math like (sizeof(float) * floatsPerVertex)
         *
         * All you need to remember is that:
         *
         * attribute in location 0 has 3 components (floats)
         * attribute in location 1 has 4 components (floats)
         * attribute in location 2 has 2 components (floats)
         * attribute in location 3 has 3 components (floats)
         *
         */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);*/

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texture));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texture));
        glEnableVertexAttribArray(4);
    }
    glBindVertexArray(0);

    // now calculate aabb for funsies
    calculateAABBPoints();
}