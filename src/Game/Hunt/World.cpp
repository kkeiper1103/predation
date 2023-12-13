//
// Created by kkeiper1103 on 12/12/23.
//

#include "World.h"
#include "glm/ext/matrix_clip_space.hpp"

World::World(btVector3 gravity) {

    setupPhysics(gravity);
}

void World::setupPhysics(btVector3 gravity) {
    config = std::make_unique<btDefaultCollisionConfiguration>();
    dispatcher = std::make_unique<btCollisionDispatcher>(config.get());
    broadphase = std::make_unique<btDbvtBroadphase>();
    solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    physics = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), config.get());
    physics->setGravity(gravity);
}

void World::update(float dt, double t) {
    physics->stepSimulation(dt);

    //
    // terrain->update(dt, t);

    hunter->update(dt);

    for(auto& animal: animals)
        animal->update(dt);
}

void World::render() {



    glm::mat4 projection = hunter->camera.GetProjectionMatrix();
    glm::mat4 view = hunter->camera.GetViewMatrix();
    auto model = glm::identity<glm::mat4>();
    auto mvp = projection * view * model;

    terrain->shader->setMat4("mvp", mvp);
    terrain->draw(0, 0);

    for(auto& animal: animals) {
        mvp = projection * view * animal->GetModel();
        animal->shader->setMat4("mvp", mvp);
        animal->draw();
    }

    mvp = projection * view * hunter->GetModel();
    hunter->shader->setMat4("mvp", mvp);
    hunter->draw();



    // draw gui
}
