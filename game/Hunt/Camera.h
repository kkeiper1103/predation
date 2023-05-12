//
// Created by kkeiper1103 on 12/23/22.
//

#ifndef PREDATION_CAMERA_H
#define PREDATION_CAMERA_H

#include <SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Hunter;

class Camera {
protected:
    friend class Hunter;
    Hunter* parent = nullptr;

public:
    glm::vec3 position {64, 64, 64};
    glm::vec3 rotation {0, 0, 0};
    glm::vec3 target {0, 0, -1};
    glm::vec3 up {0, 1, 0};

protected:
    glm::mat4 projection {1};

public:
    Camera(Hunter* parent, glm::vec3 position, glm::vec3 target = {0, 0, -1});
    ~Camera();

    void input(SDL_Event* e);
    void update(double dt);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
};


#endif //PREDATION_CAMERA_H
