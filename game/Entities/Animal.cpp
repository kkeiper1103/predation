//
// Created by kkeiper1103 on 12/18/22.
//

#include "Animal.h"

#include "Hunt/Hunt.h"

void Animal::draw() {
    printf("should be drawing %s at (%.2f, %.2f, %.2f)...\n", mesh->name, position.x, position.y, position.z);

    printf("VAO ID: %d\n", mesh->vaoId);


    mesh->shader->setMat4("view", parent->getViewMatrix());
    mesh->shader->setMat4("projection", parent->getProjectionMatrix());
    mesh->shader->setMat4("model", model);

    mesh->draw();
}

void Animal::update(double dt) {
    // @todo actually put this in the right spot
    /**
     * The issue is that when there are no animals in range, draw() doesn't get called
     * but if draw() doesn't get called (and by extension the mvp matrices set), the terrain scenery stops drawing in the correct location
     * this is a simple way to make sure it's set, but results in way more calls to glUniform** than there needs to be
     */
    mesh->shader->setMat4("view", parent->getViewMatrix());
    mesh->shader->setMat4("projection", parent->getProjectionMatrix());
    mesh->shader->setMat4("model", model);
}
