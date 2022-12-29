//
// Created by kkeiper1103 on 12/18/22.
//

#ifndef PREDATION_ANIMAL_H
#define PREDATION_ANIMAL_H

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <memory>
#include "EntityMesh.h"

class Hunt;

class Animal {
protected:
    glm::mat4 model{1.f};
    Hunt* parent{nullptr};

public:
    float scale = .02f;
    glm::vec3 rotation{0, 0, 0};
    glm::vec3 position{0, 0, 0};

public:
    // unowned pointer. must exist, DO NOT DELETE
    EntityMesh* mesh = nullptr;

    Animal(Hunt* parent, EntityMesh* mesh, glm::vec3 position) : parent(parent), mesh(mesh), position(position) {
        model = glm::translate(model, position);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3{1, 0, 0});
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3{0, 1, 0});
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3{0, 0, 1});

        model = glm::scale(model, glm::vec3(scale, scale, scale));
    }

    void update(double dt);
    void draw();
};

typedef std::shared_ptr<Animal> AnimalPtr;

#endif //PREDATION_ANIMAL_H
