//
// Created by kkeiper1103 on 12/18/22.
//

#ifndef PREDATION_HUNT_H
#define PREDATION_HUNT_H

#include <SDL2/SDL.h>
#include <nuklear/nuklear.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <easyloggingpp/easylogging++.h>
#include <ocarn2.h>

#include <vector>

#include "AreaEntry.h"
#include "WeaponEntry.h"
#include "AnimalEntry.h"
#include "Entities/Animal.h"
#include "Hunt/Terrain/ChunkedTerrain.h"
#include "Entities/Skybox.h"

// forward declaration for parent/child relationship
class GameWorld;

struct Player {
    glm::vec3 position{0, 0, 0};
};

// testing
class GLMesh {
public:
    GLuint vaoId{0};
    GLuint vboId{0};
    GLuint textureId{0};

    friend class Hunt;
    std::shared_ptr<Shader> shader;

    struct MeshVertex {
        MeshVertex(float x, float y, float z) : position(x, y, z) {}
        glm::vec3 position;
    };

    explicit GLMesh(OCARN2::Mesh* data) {
        glGenVertexArrays(1, &vaoId);
        glGenBuffers(1, &vboId);
        glGenTextures(1, &textureId);

        shader = Shader::FromFiles("resources/shaders/Mesh.vert", "resources/shaders/Mesh.frag");

        std::vector<MeshVertex> vertices;

        for(auto const& face: data->faces) {
            auto const& v1 = data->vertices[ face.v1 ],
                 &v2 = data->vertices[ face.v2 ],
                 &v3 = data->vertices[ face.v3 ];

            vertices.emplace_back(v1.x, v1.y, v1.z);
            vertices.emplace_back(v2.x, v2.y, v2.z);
            vertices.emplace_back(v3.x, v3.y, v3.z);
        }

        glBindVertexArray(vaoId);
        {
            glBindBuffer(GL_ARRAY_BUFFER, vboId);
            glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*) offsetof(MeshVertex, position));
            glEnableVertexAttribArray(0);
        }
        glBindVertexArray(0);
    }
    ~GLMesh() {
        glDeleteVertexArrays(1, &vaoId);
        glDeleteBuffers(1, &vboId);
        glDeleteTextures(1, &textureId);
    }

    void draw() {
        shader->use();

        glBindVertexArray(vaoId);
        {
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glBindVertexArray(0);
    }
};




class Hunt {
public:
    // @todo move camera to player class
    // as the camera will always be relative to the player
    glm::vec3 cameraPosition{0, 0, 100};
    glm::vec3 cameraTarget{0, 0, -1};
    glm::vec3 cameraUp{0, 1, 0};
    glm::mat4 view{1};
    glm::mat4 projection{1};

private:
    GameWorld* parent = nullptr;
    OCARN2::Map map;
    OCARN2::Rsc rsc;

    std::unique_ptr<ChunkedTerrain> terrain;

    std::vector<OCARN2::Mesh> animalMeshes;
    std::vector<OCARN2::Mesh> weaponMeshes;

    //
    Player player;
    std::vector<AnimalPtr> animals;

    std::unique_ptr<Skybox> skybox = nullptr;

    // test mesh
    std::unique_ptr<GLMesh> mesh = nullptr;

public:
    // used to show the pause menu stuff
    bool isPaused = false;
    // used to alert GameWorld that the Hunt is over
    bool isComplete = false;

    Hunt(GameWorld* parent, const AreaEntry& area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons);
    ~Hunt();

    void input(SDL_Event* e);
    void update(double dt);
    void gui(nk_context* ctx);
    void render();

    void DrawTerrainChunkAt(int x, int z);
    void UpdateAnimal(const Animal& animal);
    void DrawAnimal(const Animal& animal);
    void DrawPlayer();
    void DrawSkybox();

    std::vector<Animal*> GetAnimalsInRadius(int x, int z, float radius);
};


#endif //PREDATION_HUNT_H
