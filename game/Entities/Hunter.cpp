//
// Created by kkeiper1103 on 12/18/22.
//

#include "Hunter.h"

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
}

void Hunter::draw() {
    shader->use();


}

Hunter::Hunter(glm::vec3 position) : position{position} {

}

Hunter::~Hunter() {

}
