//
// Created by kkeiper1103 on 12/18/22.
//

#include "Hunter.h"

#include "Hunt/Hunt.h"
#include "Controls/KeyboardController.h"
#include "Controls/GamepadController.h"

void Hunter::input(SDL_Event *e) {
    static float yaw = -90.f;
    static float pitch = 25.f;

    // not sure?
    if( controller->isActionPresent(Action::ACTION_LOOK_HORIZONTAL) || controller->isActionPresent(Action::ACTION_LOOK_VERTICAL) ) {
        float xrel = controller->getActionMagnitude(Action::ACTION_LOOK_HORIZONTAL);
        float yrel = controller->getActionMagnitude(Action::ACTION_LOOK_VERTICAL);

        yaw += xrel / 2.f;
        pitch -= yrel;
    }

    if(e->type == SDL_MOUSEMOTION) {
        yaw += e->motion.xrel / 2.f;
        pitch -= e->motion.yrel;
    }

    pitch = std::clamp(pitch, -89.f, 89.f);

    rotation.x = cos(glm::radians(yaw));
    rotation.z = sin(glm::radians(yaw));
    rotation.y = sin(glm::radians(pitch));
}

void Hunter::update(double dt) {

    // move mesh downwards
    position.y -= dt * velocity.y;
    position.y = std::clamp(position.y, parent->terrain->GetHeight(position.x, position.z), 256.f);

    // handle x movement
    if( controller->isActionPresent(Action::ACTION_RIGHT) )
        position += controller->getActionMagnitude(Action::ACTION_RIGHT) * glm::normalize(glm::cross(camera.target, camera.up)) * velocity.x * (float) dt;
    else if( controller->isActionPresent(Action::ACTION_LEFT) )
        position -= controller->getActionMagnitude(Action::ACTION_LEFT) * glm::normalize(glm::cross(camera.target, camera.up)) * velocity.x * (float) dt;

    // handle z movement
    if( controller->isActionPresent(Action::ACTION_FORWARD) )
        position += controller->getActionMagnitude(Action::ACTION_FORWARD) * velocity.z * camera.target * (float) dt;
    else if( controller->isActionPresent(Action::ACTION_BACKWARD) )
        position -= controller->getActionMagnitude(Action::ACTION_BACKWARD) * velocity.z * camera.target * (float) dt;

    // handle making the player jump (only if the player isn't already in the air
    if(controller->isActionPresent(Action::ACTION_JUMP) && onGround) {
        onGround = false;
        jumpTimer = jumpDuration;

        velocity.y = -9.8;
    }

    // this slows the player as they go higher in the air
    if(!onGround) {
        velocity.y += dt * 20;
        jumpTimer -= dt;
    }

    // if the jump is over, reset velocity back to gravity
    if(jumpTimer <= 0.f) {
        velocity.y = 9.8;

        // if the difference in player y and terrain y is negligible, we're on ground
        if( abs(position.y - parent->terrain->GetHeight(position.x, position.z)) < 0.01 )
            onGround = true;
    }

    // toggle weapon
    if(controller->isActionPresent(Action::ACTION_TOGGLE_WEAPON)) {
        weaponReady = !weaponReady;

        printf("Weapon Ready: %d\n", (int) weaponReady);
    }

    // fire weapon
    if(weaponReady && controller->isActionPresent(Action::ACTION_FIRE_WEAPON)) {
        printf("Firing Weapon!\n");
    }

    position.x = std::clamp(position.x, 1.f, 1023.f);
    position.z = std::clamp(position.z, 1.f, 1023.f);

    camera.update(dt);
}

void Hunter::draw() {
    if(!shader) return; // if there's no shader, there's no sense in rendering the player

    shader->use();
}

Hunter::Hunter(Hunt* parent, glm::vec3 position) :
    parent(parent),
    position{position},
    camera(this, position, rotation)
{
    controller = std::make_unique<KeyboardController>();

    if(SDL_IsGameController(0) == SDL_TRUE)
        controller = std::make_unique<GamepadController>(0);
}

Hunter::~Hunter() {

}
