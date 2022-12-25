//
// Created by kkeiper1103 on 12/23/22.
//

#ifndef PREDATION_CAMERA_H
#define PREDATION_CAMERA_H

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Hunt;

class Camera {
protected:
    Hunt* parent = nullptr;

    glm::vec3 position {64, 64, 64};
    glm::vec3 target {0, -1, -1};
    glm::vec3 up {0, 1, 0};

    glm::mat4 projection {1};

public:
    Camera(Hunt* parent, glm::vec3 position, glm::vec3 target = {0, 0, -1});
    ~Camera();

    void input(SDL_Event* e);
    void update(double dt);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
};


#endif //PREDATION_CAMERA_H
