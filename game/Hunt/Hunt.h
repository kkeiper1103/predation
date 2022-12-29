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
#include "Entities/EntityMesh.h"
#include "Camera.h"
#include "Entities/Hunter.h"

// forward declaration for parent/child relationship
class GameWorld;

class Hunt {
private:
    friend class Hunter;
    GameWorld* parent = nullptr;
    OCARN2::Map map;
    OCARN2::Rsc rsc;

    std::unique_ptr<ChunkedTerrain> terrain;

    std::vector<OCARN2::Mesh> animalMeshes;
    std::vector<OCARN2::Mesh> weaponMeshes;


    std::unique_ptr<Hunter> hunter;

    std::vector<AnimalPtr> animals;

    std::unique_ptr<Skybox> skybox = nullptr;

    // test mesh
    std::unique_ptr<EntityMesh> mesh = nullptr;

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
    void UpdateAnimal( Animal* animal );
    void DrawAnimal( Animal* animal );
    void DrawPlayer();
    void DrawSkybox();

    std::vector<Animal*> GetAnimalsInRadius(int x, int z, float radius);

    void DecorateTerrain();

    void DrawSceneryWithinRadius(int radius);
};


#endif //PREDATION_HUNT_H
