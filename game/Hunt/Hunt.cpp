//
// Created by kkeiper1103 on 12/18/22.
//

#include "Hunt.h"

#include <random>

#include "World/GameWorld.h"

#include "Application.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "Hunt/Terrain/SmoothedTerrain.h"


EntityMesh* Hunt::GetMeshForCharacter(OCARN2::Mesh* character) {
    if( !characters.contains(character->name) ) {
        characters[character->name] = std::make_unique<EntityMesh>(character);
    }

    return characters[character->name].get();
}


Hunt::Hunt(GameWorld* parent, const AreaEntry &area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons) : parent(parent) {
    map = load_map_file(area.mapFile.c_str());
    rsc = load_rsc_file(area.rscFile.c_str());

    // since the heightmap is a 1d array, this gives us one side's length of the map
    // for carn2 and ice, this *should* be 1024
    int mapsize = ceil(sqrt(map.heightMap.size()));

    // now we do it again to make it chunked into smaller sizes
    // carn2 and ice age should give us 32 chunks across
    terrain = std::make_unique<ChunkedTerrain>(&map, &rsc, ceil(sqrt(mapsize)));


    animalMeshes.resize(animals.size());
    for(auto i=0; i < animals.size(); i++) {
        animalMeshes[i] = load_car_file(animals[i].file.c_str());
    }

    weaponMeshes.resize(weapons.size());
    for(auto i=0; i < weapons.size(); i++)
        weaponMeshes[i] = load_car_file(weapons[i].modelFile.c_str());

    // set up rng
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(20, 40);

    // spawn animals
    int spawnLimit = distribution(gen);
    this->animals.reserve(spawnLimit);

    int xPosition=0;
    std::uniform_int_distribution<> animalTypeDistribution(0, animalMeshes.size() - 1);
    for( auto i=0; i < spawnLimit; i++ ) {
        auto type = animalMeshes[ animalTypeDistribution(gen) ];

        // @todo come up with better, more informative name. Too much confusion between OCARN2::Mesh and EntityMesh
        // one is for model data, the other is for gl buffered data
        auto characterMesh = GetMeshForCharacter(&type);

        this->animals.push_back( std::make_shared<Animal>(characterMesh, glm::vec3 { xPosition++ * 200, 0, 200 }) );
    }

    // make skybox
    skybox = std::make_unique<Skybox>( rsc.sky[parent->huntConfig.timeOfDay] );

    // make mesh for testing
    mesh = std::make_unique<EntityMesh>( &animalMeshes[0] );

    // set the player's position to one of the spawn points
    player.position = terrain->GetRandomSpawnLocation();


    //
    hunter = std::make_unique<Hunter>(terrain->GetRandomSpawnLocation());
}

Hunt::~Hunt() {
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
    }

    // don't move the camera when the pause menu is up, or else they'll be like WUT
    if(!isPaused) {
        hunter->input(e);

        if(e->type == SDL_MOUSEMOTION) {
            static float yaw = -90.f;
            static float pitch = 25.f;

            yaw += e->motion.xrel / 2.f;
            pitch -= e->motion.yrel;

            pitch = std::clamp(pitch, -89.f, 89.f);

            player.rotation.x = cos(glm::radians(yaw));
            player.rotation.z = sin(glm::radians(yaw));
            player.rotation.y = sin(glm::radians(pitch));
        }
    }
}

void Hunt::update(double dt) {
    if(isPaused) return;

    terrain->shader->setVec3("viewPosition", camera.position);

    skybox->shader->setMat4("view", glm::mat4(glm::mat3( camera.GetViewMatrix() )));
    skybox->shader->setMat4("projection", camera.GetProjectionMatrix());

    mesh->shader->setMat4("view", camera.GetViewMatrix());
    mesh->shader->setMat4("projection", camera.GetProjectionMatrix());
    mesh->shader->setMat4("model", glm::mat4(1.f));

    for(auto& animal: animals) {
        UpdateAnimal( animal.get() );
    }

    // move player position for fun, testing
    auto keys = SDL_GetKeyboardState(nullptr);

    if(keys[SDL_SCANCODE_F6]) {
        printf( "Height At (%.2f, %.2f): %.3f\n", player.position.x, player.position.z, terrain->GetHeight(player.position.x, player.position.z) );
        printf("Height At (%d, %d): %d\n",
               (int) floor(player.position.x),
               (int) floor(player.position.z),
               (GLushort) terrain->GetHeight((int) floor(player.position.x), (int) floor(player.position.z))
           );

        printf("Height At (%d, %d): %d\n",
               (int) floor(player.position.x) + 1,
               (int) floor(player.position.z) + 1,
               (GLushort) terrain->GetHeight((int) floor(player.position.x) + 1, (int) floor(player.position.z) + 1)
        );
    }

    const float playerSpeed = 5.f;
    /**
     * @todo add slope checking *in the direction of the player*
     */
    if( keys[SDL_SCANCODE_D] ) {
        player.position += glm::normalize(glm::cross(camera.target, camera.up)) * playerSpeed * (float) dt;
    }
    else if( keys[SDL_SCANCODE_A] ) {
        player.position -= glm::normalize(glm::cross(camera.target, camera.up)) * playerSpeed * (float) dt;
    }

    if( keys[SDL_SCANCODE_W] )
        player.position += playerSpeed * camera.target * (float) dt;
    else if( keys[SDL_SCANCODE_S] )
        player.position -= playerSpeed * camera.target * (float) dt;

    player.position.x = std::clamp(player.position.x, 1.f, 1023.f);
    player.position.z = std::clamp(player.position.z, 1.f, 1023.f);

    player.position.y = terrain->GetHeight(player.position.x, player.position.z);

    camera.update(dt);
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
    if(isPaused) return;


    mesh->draw();


    DrawTerrainChunkAt((int) floor(player.position.x), (int) floor(player.position.z));


    for(auto& animalPtr: GetAnimalsInRadius((int) floor(player.position.x), (int) floor(player.position.z), 30.f)) {
        // DrawAnimal( animalPtr );
    }
/*
    DrawPlayer();*/

    // render after everything else is filled, so we only fill pixels that haven't been rendered to
    DrawSkybox();
}

void Hunt::DrawTerrainChunkAt(int x, int z) {
    // @todo make configurable, rather than hard coded.
    // this just does bounds checking to make sure we don't accidentally get a chunk out of range
    if(x < 0 || z < 0 || x > 1023 || z > 1023) return;

    int chunkX = floor( x / terrain->chunksPerSide );
    int chunkZ = floor( z / terrain->chunksPerSide );

    terrain->shader->setMat4("view", camera.GetViewMatrix());
    terrain->shader->setMat4("projection", camera.GetProjectionMatrix());
    terrain->shader->setMat4("model", glm::mat4(1.f));

    terrain->draw(chunkX, chunkZ);
}

void Hunt::UpdateAnimal( Animal* animal ) {
    animal->mesh->shader->setMat4("projection", camera.GetProjectionMatrix());
    animal->mesh->shader->setMat4("view", camera.GetViewMatrix());
}

void Hunt::DrawAnimal( Animal* animal ) {
    animal->draw();
}

void Hunt::DrawPlayer() {

}

std::vector<Animal*> Hunt::GetAnimalsInRadius(int x, int z, float radius) {
    static std::vector<Animal*> retval; retval.clear();

    // @todo add distance checking once positions are implemented and only push back any animals within radius
    for(auto& animal: animals) {
        retval.push_back(animal.get());
    }

    return retval;
}

void Hunt::DrawSkybox() {
    skybox->draw();
}
