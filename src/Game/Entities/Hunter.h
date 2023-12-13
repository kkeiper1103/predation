//
// Created by kkeiper1103 on 12/18/22.
//

#ifndef PREDATION_HUNTER_H
#define PREDATION_HUNTER_H

#include <glm/glm.hpp>
#include <SDL_events.h>
#include "EntityMesh.h"
#include "Hunt/Camera.h"

#include <memory>
#include "Controls/Controller.h"
#include "collisions/CollisionShape.h"

class Hunt;

class Hunter {
protected:
    // since we're actually loading the geometry in this class, we'll own this pointer, as opposed to the Animal class
    std::unique_ptr<EntityMesh> mesh = nullptr;

    // unowned pointer. do not delete
    Hunt* parent = nullptr;

    friend class Camera;

    glm::mat4 model = glm::mat4(1.f);

public:
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 velocity{5, 9.8, 5};


    static std::unique_ptr<Shader> shader2;

    std::shared_ptr<Shader> shader;
    std::unique_ptr<Controller> controller;

    // jump information
    bool onGround = true;
    double jumpDuration = 0.25;
    double jumpTimer = 0;

    // weapon information
    bool weaponReady{false};

public: // collision stuff
    CollisionShape* bounds = nullptr;

public:
    explicit Hunter(Hunt* parent, glm::vec3 position);
    ~Hunter();

    void input(SDL_Event* e);
    void update(double dt);
    void draw();

    [[nodiscard]] glm::mat4 GetModel() const { return model; }

    Camera camera;
};


#endif //PREDATION_HUNTER_H
