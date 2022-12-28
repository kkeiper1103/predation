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
    position.y = std::clamp(position.y, 0.f, 256.f);


    const float playerSpeed = 5.f;
    /**
     * @todo add slope checking *in the direction of the player*
     */
    if( controller->isActionPresent(Action::ACTION_RIGHT) ) {
        position += glm::normalize(glm::cross(camera.target, camera.up)) * playerSpeed * (float) dt;
    }
    else if( controller->isActionPresent(Action::ACTION_LEFT) ) {
        position -= glm::normalize(glm::cross(camera.target, camera.up)) * playerSpeed * (float) dt;
    }

    if( controller->isActionPresent(Action::ACTION_FORWARD) )
        position += playerSpeed * camera.target * (float) dt;
    else if( controller->isActionPresent(Action::ACTION_BACKWARD) )
        position -= playerSpeed * camera.target * (float) dt;

    position.x = std::clamp(position.x, 1.f, 1023.f);
    position.z = std::clamp(position.z, 1.f, 1023.f);


    camera.update(dt);
}

void Hunter::draw() {
    shader->use();


}

Hunter::Hunter(Hunt* parent, glm::vec3 position) :
parent(parent),
position{position},
camera(parent, position, rotation) {

    controller = std::make_unique<KeyboardController>();
}

Hunter::~Hunter() {

}
