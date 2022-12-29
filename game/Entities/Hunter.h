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

class Hunt;

class Hunter {
protected:
    // unowned pointer. do not delete
    EntityMesh* mesh = nullptr;
    Hunt* parent = nullptr;

    friend class Camera;

    glm::mat4 model = glm::mat4(1.f);

public:
    glm::vec3 position{0, 0, 0};
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 velocity{5, 9.8, 5};


    std::shared_ptr<Shader> shader;
    std::unique_ptr<Controller> controller;

    // jump information
    bool onGround = true;
    double jumpDuration = 0.25;
    double jumpTimer = 0;

    // weapon information
    bool weaponReady{false};

public:
    explicit Hunter(Hunt* parent, glm::vec3 position);
    ~Hunter();

    void input(SDL_Event* e);
    void update(double dt);
    void draw();

    Camera camera;
};


#endif //PREDATION_HUNTER_H
