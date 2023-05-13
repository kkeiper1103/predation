//
// Created by kkeiper1103 on 5/13/2023.
//

#ifndef PREDATION_TRANSFORM_H
#define PREDATION_TRANSFORM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {
    glm::vec3 position {0, 0, 0};
    glm::vec3 anchor {.5, .5, .5};
    glm::vec3 rotation {0, 0, 0};
    glm::vec3 scale {0, 0, 0};

    /**
     *
     * @param out
     */
    inline void mat4(glm::mat4& out) {
        out = glm::translate(out, position);

        auto quaternion = glm::quat( glm::vec3 {
            glm::radians(rotation.x),
            glm::radians(rotation.y),
            glm::radians(rotation.z)
        });

        out = out * glm::mat4_cast(quaternion);

        out = glm::scale(out, scale);
    }
};

#endif //PREDATION_TRANSFORM_H
