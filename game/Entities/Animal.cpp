//
// Created by kkeiper1103 on 12/18/22.
//

#include "Animal.h"

void Animal::draw() {
    mesh->shader->setMat4("model", model);

    mesh->draw();
}
