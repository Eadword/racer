#include "entity.h"

#include <glm/gtc/matrix_transform.hpp>

void MobileEntity::updateMobVals(glm::vec3* npyr, glm::vec3* npos, glm::vec3* nup) {
    if(npyr != nullptr) pyr = *npyr;
    if(npos != nullptr) position = *npos;
    if(nup != nullptr) up = glm::normalize(*nup);

    // TODO: integrate pitch and roll
    direction = glm::vec3(glm::cos(pyr.y), 0, glm::sin(pyr.y));

    glm::vec3 u, v, n;
    n = -1.0f * direction;
    u = glm::normalize(glm::cross(up, n));
    v = glm::normalize(glm::cross(n, u));

    mob_transform[0] = glm::vec4(u, 0);
    mob_transform[1] = glm::vec4(v, 0);
    mob_transform[2] = glm::vec4(n, 0);
    mob_transform[3] = glm::vec4(position, 1);
}

void MobileEntity::move(float t) {
    position += direction * t;
    updateMobVals();
}

void MobileEntity::turn(float angle) {
    pyr.y += angle;
    if(pyr.y > glm::two_pi<float>()) pyr.y -= glm::two_pi<float>();

    updateMobVals();
}
