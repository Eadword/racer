#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>

#include "camera.h"

Camera::Camera() {
    setViewVolume(
        45.0f, // Fovy
        4.0f / 3.0f,                  // Aspect
        1.0f,                         // Near
        10000.0f                      // Far
    );
}

void Camera::orient(const glm::vec3& position, const glm::vec3& at, const glm::vec3& up) {
    this->position = position;
    n = glm::normalize(position - at);
    u = glm::normalize(glm::cross(up, n));
    v = glm::normalize(glm::cross(n, u));
}

void Camera::slide(const glm::vec3& d) {
    position += d.x * u + d.y * v + d.z * n;
}

void Camera::slideXZ(const glm::vec3& d) {
    float factor = 1 / getTopAngle(v); // Keep it moving at same speed even when facing downwards
    glm::vec3 tmp = d.x * u + d.y * v + d.z * n * factor;
    tmp.y = 0;
    position += tmp;
}

void Camera::slideY(const glm::vec3& d) {
    float factor = 1 / getTopAngle(v);
    glm::vec3 tmp = d.x * u + d.y * v * factor + d.z * n;
    tmp.x = 0;
    tmp.z = 0;
    position += tmp;
}

void Camera::rotate(float angle, const glm::vec3& axis) {
    glm::mat4 t = glm::rotate(glm::mat4(), angle, axis);
    glm::mat3 r(t);

    u = glm::normalize(r * u);
    v = glm::normalize(r * v);
    n = glm::normalize(r * n);
}

void Camera::pitch(float angle) {
    glm::vec3 v2 = v * glm::cos(angle) + n * glm::sin(angle);
    glm::vec3 n2 = n * glm::cos(angle) - v * glm::sin(angle);

    // Prevent camera going upside down
    if(getTopAngle(v2) <= 0.0f) return;

    v = glm::normalize(v2);
    n = glm::normalize(n2);
    u = glm::normalize(glm::cross(v, n));
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(fovy, aspect, near_plane, far_plane);
}

glm::mat4 Camera::getViewMatrix() const {
    glm::mat4 rotate(
        glm::vec4(u, 0),
        glm::vec4(v, 0),
        glm::vec4(n, 0),
        glm::vec4(0, 0, 0, 1.0f)
    );
    rotate = glm::transpose(rotate); //inverse == transpose
    glm::mat4 translate;
    translate[3] = glm::vec4(-1.0f * position, 1.0f);

    glm::mat4 view = rotate * translate;

    return view;
}
