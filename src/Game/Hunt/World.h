//
// Created by kkeiper1103 on 12/12/23.
//

#pragma once

#include <vector>
#include <map>

#include <memory>

#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"


#include "Entities/Animal.h"
#include "Entities/Hunter.h"
#include "Hunt/Terrain/ITerrain.h"

class World {
public:
    explicit World(btVector3 gravity = {0, -9.8, 0});


    void update(float dt, double t);
    void render();


    std::shared_ptr<ITerrain> terrain;
    std::vector< std::shared_ptr<Animal> > animals;
    std::shared_ptr<Hunter> hunter;

protected:
    std::unique_ptr<btDefaultCollisionConfiguration> config;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btDbvtBroadphase> broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::shared_ptr<btDiscreteDynamicsWorld> physics;

    // std::vector<Characters> characters;
    void setupPhysics(btVector3 vector3);
};
