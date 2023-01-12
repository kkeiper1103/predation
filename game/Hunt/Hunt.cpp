//
// Created by kkeiper1103 on 12/18/22.
//

#include "Hunt.h"

#include <random>

#include "Application/Kernel.h"

#include "Application.h"


EntityMesh* Hunt::GetMeshForCharacter(OCARN2::Mesh* character) {
    if( !characters.contains(character->name) ) {
        characters[character->name] = std::make_unique<EntityMesh>(character);
    }

    return characters[character->name].get();
}


Hunt::Hunt(Kernel* parent, const AreaEntry &area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons) : parent(parent) {
    map = load_map_file(area.mapFile.c_str());
    rsc = load_rsc_file(area.rscFile.c_str());

    // since the heightmap is a 1d array, this gives us one side's length of the map
    // for carn2 and ice, this *should* be 1024
    int mapsize = ceil(sqrt(map.heightMap.size()));

    // now we do it again to make it chunked into smaller sizes
    // carn2 and ice age should give us 32 chunks across
    terrain = std::make_unique<ChunkedTerrain>(&map, &rsc, ceil(sqrt(mapsize)));

    // spawn all the meshes and models from the terrain
    DecorateTerrain();


    animalMeshes.resize(animals.size());
    for(auto i=0; i < animals.size(); i++) {
        animalMeshes[i] = load_car_file(animals[i].file.c_str());
    }

    weaponMeshes.resize(weapons.size());
    for(auto i=0; i < weapons.size(); i++)
        weaponMeshes[i] = load_car_file(weapons[i].modelFile.c_str());

    // set up rng
    gen = std::mt19937(rd());

    // spawn animals
    SpawnAnimals(10, 25);

    // make skybox
    skybox = std::make_unique<Skybox>( rsc.sky[parent->huntConfig.timeOfDay] );

    // set the hunter's location
    hunter = std::make_unique<Hunter>(this,terrain->GetRandomSpawnLocation());

    // move mouse to center and set relative?
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_WarpMouseInWindow(app()->window, app()->config.width / 2.f, app()->config.height / 2.f);
}

Hunt::~Hunt() {
    SDL_SetRelativeMouseMode(SDL_FALSE);

    LOG(INFO) << "Unloading Resources";

    free_rsc(rsc);

    for(auto& mesh: animalMeshes)
        free_mesh(mesh);

    for(auto& mesh: weaponMeshes)
        free_mesh(mesh);
}


void Hunt::input(SDL_Event *e) {
    if(e->type == SDL_KEYUP && e->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        isPaused = !isPaused;

        SDL_ShowCursor(isPaused);
        SDL_SetRelativeMouseMode((SDL_bool) !isPaused);
    }

    // don't move the camera when the pause menu is up, or else they'll be like WUT
    if(!isPaused) {
        hunter->input(e);
    }
}

void Hunt::update(double dt) {
    if(isPaused) return;

    terrain->shader->setVec3("viewPosition", hunter->camera.position);

    skybox->update(dt);
    skybox->shader->setMat4("view", glm::mat4(glm::mat3( hunter->camera.GetViewMatrix() )));
    skybox->shader->setMat4("projection", hunter->camera.GetProjectionMatrix());

    hunter->update(dt);

    for(auto& animal: animals) {
        animal->update(dt);
    }
}

void Hunt::gui(nk_context *ctx) {
    static auto config = &app()->config;

    if(isPaused) {
        static int width = 300;
        static int height = 500;
        if(nk_begin(ctx, "Pause Menu", nk_rect( config->width / 2 - width / 2, config->height / 2 - height / 2, width, height ), NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_TITLE)) {

            nk_layout_row_dynamic(ctx, 35, 1);

            if(nk_button_label(ctx, "Return to Main Menu")) {
                isComplete = true;
            }
            if(nk_button_label(ctx, "Exit Game")) {
                app()->running = false;
            }

        } nk_end(ctx);
    }
}

void Hunt::render() {

    // draw the containing chunk, as well as a radius around it
    DrawTerrainChunkAt((int) floor(hunter->position.x), (int) floor(hunter->position.z));

    // get all terrain objects in radius
    DrawSceneryWithinRadius(GameSettings::Get().viewRadius);

    // @todo
    for(auto& animal: GetAnimalsInRadius((int) floor(hunter->position.x), (int) floor(hunter->position.z), GameSettings::Get().viewRadius)) {
        animal->draw();
    }

    DrawPlayer();

    // render after everything else is filled, so we only fill pixels that haven't been rendered to
    DrawSkybox();
}

void Hunt::DrawTerrainChunkAt(int x, int z) {
    // @todo make configurable, rather than hard coded.
    // this just does bounds checking to make sure we don't accidentally get a chunk out of range
    if(x < 0 || z < 0 || x > 1023 || z > 1023) return;

    int chunkX = floor( x / terrain->chunksPerSide );
    int chunkZ = floor( z / terrain->chunksPerSide );

    terrain->shader->setMat4("view", hunter->camera.GetViewMatrix());
    terrain->shader->setMat4("projection", hunter->camera.GetProjectionMatrix());
    terrain->shader->setMat4("model", glm::mat4(1.f));

    terrain->draw(chunkX, chunkZ);
}

void Hunt::DrawPlayer() {
    hunter->draw();
}

std::vector<AnimalPtr> Hunt::GetAnimalsInRadius(int x, int z, float radius) {
    static std::vector<AnimalPtr> retval; retval.clear();


    // @todo add distance checking once positions are implemented and only push back any animals within radius
    for(auto& animal: animals) {
        float distance = sqrt(
            pow((animal->position.x - hunter->position.x), 2) +
            pow((animal->position.y - hunter->position.y), 2) +
            pow((animal->position.z - hunter->position.z), 2)
        );


        if(distance < radius) {
            retval.push_back(animal);
        }
    }

    return retval;
}

void Hunt::DrawSkybox() {
    skybox->draw();
}

void Hunt::DecorateTerrain() {
    const int MAP_SIZE = 1024;

    terrainModelUniforms.resize(rsc.numModels);
    terrainModelMat4s.resize(rsc.numModels);
    terrainModels.resize(rsc.numModels);

    for(int z=0; z < MAP_SIZE; z++) {
        for(int x=0; x < MAP_SIZE; x++) {
            int idx = z * MAP_SIZE + x;

            int objectId = map.objectMap[idx],
                objectHeight = map.objectHeightMap[idx],
                objectRotation = (map.bitflagMap[idx] & OCARN2::BF_MODEL_DIRECTION);

            // if the object id is lower than the size of the models array, we have a valid model id
            if( objectId < rsc.numModels ) {
                // for instanced rendering
                terrainModelUniforms[objectId].push_back({
                    .position = { x, objectHeight, z },
                    .rotation = { 0, glm::radians(objectRotation * -90.f), 0}
                });
            }

            // now, build the model. it's a pointer, so if it's null, it hasn't been built
            if( objectId < 254 && !terrainModels[objectId] ) {
                auto& model = terrainModels[objectId];
                auto& settings = rsc.models[objectId];

                terrainModels[objectId] = std::make_unique<EntityMesh>(&settings.mesh);
            }
        }
    }
}

void Hunt::DrawSceneryWithinRadius(int radius) {
    for(int z = floor(hunter->position.z) - radius / 2; z < floor(hunter->position.z) + radius / 2; z++) {
        for(int x = floor(hunter->position.x) - radius / 2; x < floor(hunter->position.x) + radius / 2; x++) {
            int idx = z * 1024 + x;

            // if the item is not a spawn point, draw it
            if( map.objectMap[idx] < 254 ) {
                terrainModels[ map.objectMap[idx] ]->drawAt(x, terrain->GetHeight(idx), z);
            }
        }
    }
}

void Hunt::SpawnAnimals(int min, int max) {
    std::uniform_int_distribution<> distribution(min, max);

    int spawnLimit = distribution(gen);
    printf("Spawning %d Animals\n", spawnLimit);
    this->animals.reserve(spawnLimit);


    std::uniform_int_distribution<> animalTypeDistribution(0, animalMeshes.size() - 1);
    std::uniform_int_distribution<> animalPositionDistribution(0, 1023); // between 0 and map_size
    for( auto i=0; i < spawnLimit; i++ ) {
        auto type = animalMeshes[ animalTypeDistribution(gen) ];

        // @todo come up with better, more informative name. Too much confusion between OCARN2::Mesh and EntityMesh
        // one is for model data, the other is for gl buffered data
        auto characterMesh = GetMeshForCharacter(&type);


        float x = animalPositionDistribution(gen),
            z = animalPositionDistribution(gen),
            y = terrain->GetHeight(x, z);

        printf("Spawning %s at (%.2f, %.2f, %.2f)...\n", type.name, x, y, z);
        this->animals.push_back( std::make_shared<Animal>(this,characterMesh, glm::vec3 { x, y, z }) );
    }
}
