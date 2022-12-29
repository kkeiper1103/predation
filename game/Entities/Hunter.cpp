//
// Created by kkeiper1103 on 12/18/22.
//

#include "Hunter.h"

#include "Hunt/Hunt.h"
#include "Controls/KeyboardController.h"

void Hunter::input(SDL_Event *e) {
    if(e->type == SDL_MOUSEMOTION) {
        static float yaw = -90.f;
        static float pitch = 25.f;

        yaw += e->motion.xrel / 2.f;
        pitch -= e->motion.yrel;

        pitch = std::clamp(pitch, -89.f, 89.f);

        rotation.x = cos(glm::radians(yaw));
        rotation.z = sin(glm::radians(yaw));
        rotation.y = sin(glm::radians(pitch));
    }
}

void Hunter::update(double dt) {

    // move mesh downwards
    position.y -= dt * velocity.y;
    position.y = std::clamp(position.y, parent->terrain->GetHeight(position.x, position.z), 256.f);

    // handle x movement
    if( controller->isActionPresent(Action::ACTION_RIGHT) )
        position += glm::normalize(glm::cross(camera.target, camera.up)) * velocity.x * (float) dt;
    else if( controller->isActionPresent(Action::ACTION_LEFT) )
        position -= glm::normalize(glm::cross(camera.target, camera.up)) * velocity.x * (float) dt;

    // handle z movement
    if( controller->isActionPresent(Action::ACTION_FORWARD) )
        position += velocity.z * camera.target * (float) dt;
    else if( controller->isActionPresent(Action::ACTION_BACKWARD) )
        position -= velocity.z * camera.target * (float) dt;

    // handle making the player jump (only if the player isn't already in the air
    if(controller->isActionPresent(Action::ACTION_JUMP) && !inJump) {
        inJump = true;
        jumpTimer = jumpDuration;

        velocity.y = -9.8;
    }

    // this slows the player as they go higher in the air
    if(inJump) {
        velocity.y += dt * 20;
        jumpTimer -= dt;
    }

    // if the jump is over, reset velocity back to gravity
    if(jumpTimer <= 0.f) {
        inJump = false;
        velocity.y = 9.8;
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
}

Hunter::~Hunter() {

}
