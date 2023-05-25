//
// Created by kkeiper1103 on 11/6/22.
//

#include "Model.h"

Model::Model(std::shared_ptr<Mesh> mesh, const glm::vec3 &position, const glm::vec3 &rotation, float scale) :
mesh(mesh), position(position), rotation(rotation), scale(scale) {

    computeModelMatrix();
}

void Model::update(double dt) {
    computeModelMatrix();

    mesh->update(dt);
}

void Model::draw() {
    mesh->shader->use();
    mesh->shader->setMat4("model", model);

    mesh->draw();
}

void Model::computeModelMatrix() {
    // the static qualifier just means it doesn't get redeclared each time computeModelMatrix() is called. They're created once, then persisted across calls
    static glm::vec3 _position, _rotation;
    static float _scale;

    // if any of the cached properties don't match the object's properties, recompute the model matrix
    if(_position != position || _rotation != rotation || _scale != scale) {
        model = glm::mat4(1.f);
        model = glm::translate(model, position);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

        model = glm::scale(model, glm::vec3(scale, scale, scale));

        // set the static flags to prevent re-computation unless the components change value
        _position = position;
        _rotation = rotation;
        _scale = scale;
    }
}
