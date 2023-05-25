//
// Created by kkeiper1103 on 12/23/22.
//

#include "Camera.h"

#include "Entities/Hunter.h"
#include "Application.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "GameSettings.h"

Camera::Camera(Hunter* parent, glm::vec3 position, glm::vec3 target) : parent{parent}, position{position}, target{target}, up{0, 1, 0} {
    SDL_ShowCursor(SDL_DISABLE);

    float width = (&app()->config)->width,
            height = (&app()->config)->height;

    auto gameSettings = GameSettings::Get();

    // build the view and projection matrices
    // @todo get gameSettings singleton or something
    projection = glm::perspective(glm::radians( gameSettings.fov ), width / height, .1f, gameSettings.viewRadius);
}

Camera::~Camera() {
    SDL_ShowCursor(SDL_ENABLE);
}

void Camera::input(SDL_Event* e) {
}

void Camera::update(double dt) {
    rotation = parent->rotation;

    position = parent->position;
    position.y += 2;

    target = glm::normalize(rotation);
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position, position + target, up);
}

glm::mat4 Camera::GetProjectionMatrix() {
    return projection;
}
