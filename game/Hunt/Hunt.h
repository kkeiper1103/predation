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
#include <random>

#include "AreaEntry.h"
#include "WeaponEntry.h"
#include "AnimalEntry.h"
#include "Entities/Animal.h"
#include "Hunt/Terrain/ChunkedTerrain.h"
#include "Entities/Skybox.h"
#include "Entities/EntityMesh.h"
#include "Camera.h"
#include "Entities/Hunter.h"

// forward declaration for parent/child relationship
class Kernel;

class Hunt {
public:
    Kernel* parent = nullptr;
    OCARN2::Map map;
    OCARN2::Rsc rsc;

    std::unique_ptr<ChunkedTerrain> terrain;

    std::vector<OCARN2::Mesh> animalMeshes;
    std::vector<OCARN2::Mesh> weaponMeshes;

    std::unique_ptr<Hunter> hunter;
    std::vector<AnimalPtr> animals;

    std::unique_ptr<Skybox> skybox = nullptr;


    // cache map for ocarn2 meshes
    std::map<std::string, std::unique_ptr<EntityMesh>> characters;
    EntityMesh* GetMeshForCharacter(OCARN2::Mesh* character);


    // fields for the uniforms for instanced rendering of terrain decoration
    struct TerrainModelUniform {
        glm::vec3 position {0, 0, 0};
        glm::vec3 rotation {0, 0, 0};
        float scale {1.f};
    };

    std::vector< std::vector<TerrainModelUniform> > terrainModelUniforms;
    std::vector< std::vector<glm::mat4> > terrainModelMat4s;
    std::vector< std::unique_ptr<EntityMesh> > terrainModels;

    // rng handle for hunt
    std::random_device rd;
    std::mt19937 gen;

public:
    // used to show the pause menu stuff
    bool isPaused = false;
    // used to alert GameWorld that the Hunt is over
    bool isComplete = false;

    Hunt(Kernel* parent, const AreaEntry& area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons);
    ~Hunt();

    void input(SDL_Event* e);
    void update(double dt);
    void gui(nk_context* ctx);
    void render();

    void DrawTerrainChunkAt(int x, int z);
    void DrawPlayer();
    void DrawSkybox();

    std::vector<AnimalPtr> GetAnimalsInRadius(int x, int z, float radius);

    void DecorateTerrain();

    void DrawSceneryWithinRadius(int radius);

    //
    glm::mat4 getViewMatrix() { return hunter->camera.GetViewMatrix(); }
    glm::mat4 getProjectionMatrix() { return hunter->camera.GetProjectionMatrix(); }

    void SpawnAnimals(int min, int max);
};


#endif //PREDATION_HUNT_H
