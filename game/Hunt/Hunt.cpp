//
// Created by kkeiper1103 on 12/18/22.
//


#include <random>

#include "Hunt.h"
#include "World/GameWorld.h"

#include "Application.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

Hunt::Hunt(GameWorld* parent, const AreaEntry &area, std::vector<AnimalEntry> animals, std::vector<WeaponEntry> weapons) : parent(parent) {
    map = load_map_file(area.mapFile.c_str());
    rsc = load_rsc_file(area.rscFile.c_str());

    // since the heightmap is a 1d array, this gives us one side's length of the map
    // for carn2 and ice, this *should* be 1024
    int mapsize = ceil(sqrt(map.heightMap.size()));

    // now we do it again to make it chunked into smaller sizes
    // carn2 and ice age should give us 32 chunks across
    // terrain = std::make_unique<ChunkedTerrain>(&map, &rsc, ceil(sqrt(mapsize)));


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
    for( auto i=0; i < spawnLimit; i++ ) {
        this->animals.push_back( std::make_shared<Animal>() );
    }

    float width = (&app()->config)->width,
            height = (&app()->config)->height;

    // build the view and projection matrices
    view = glm::lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUp);
    projection = glm::perspective(glm::radians(parent->gameSettings.fov), width / height, .1f, 1000.f);

    // make skybox for data
    skybox = std::make_unique<Skybox>( rsc.sky[parent->huntConfig.timeOfDay] );

    // make mesh for testing
    mesh = std::make_unique<EntityMesh>( &animalMeshes[0] );
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
    }

    if(e->type == SDL_KEYUP && e->key.keysym.scancode == SDL_SCANCODE_F3) {
        printf("toggling gui\n");
        app()->renderGui = !app()->renderGui;
    }
}

void Hunt::update(double dt) {
    if(isPaused) return;

    skybox->shader->setMat4("view", view);
    skybox->shader->setMat4("projection", projection);
    skybox->shader->setMat4("model", glm::mat4(1.f));

    mesh->shader->setMat4("view", view);
    mesh->shader->setMat4("projection", projection);

    glm::mat4 model(1.f);
    model = glm::rotate(model, glm::radians(SDL_GetTicks64() / 100.f), glm::vec3(0, 1, 0));
    mesh->shader->setMat4("model", model);

    for(auto& animal: animals) {
        // UpdateAnimal( *animal );
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
    if(isPaused) return;


    mesh->draw();

    // render after everything else is filled so we only fill pixels that haven't been rendered to
    DrawSkybox();

/*
    // draw test image
    if(mesh != nullptr) mesh->draw();

    // the player's x/z maps to one of 16 chunks
    // @fixme should scenery be part of the terrain, or separate stage?
    DrawTerrainChunkAt((int) floor(player.position.x), (int) floor(player.position.z));


    for(auto& animalPtr: GetAnimalsInRadius((int) floor(player.position.x), (int) floor(player.position.z), 30.f)) {
        DrawAnimal( *animalPtr );
    }

    DrawPlayer();*/
}

void Hunt::DrawTerrainChunkAt(int x, int z) {
    // @todo make configurable, rather than hard coded.
    // this just does bounds checking to make sure we don't accidentally get a chunk out of range
    if(x < 0 || z < 0 || x > 1023 || z > 1023) return;

    int chunkX = floor( x / terrain->chunksPerSide );
    int chunkZ = floor( z / terrain->chunksPerSide );

    terrain->draw(chunkX, chunkZ);
}

void Hunt::UpdateAnimal(const Animal& animal) {

}

void Hunt::DrawAnimal(const Animal& animal) {

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
