#include <glm/gtc/constants.hpp>

#include "shapes.h"

glm::vec3 Cone::calculateNormal(GLfloat sin, GLfloat cos) {
    glm::vec3 up(cos, sin, 0.0f);
    up = glm::normalize(up - glm::vec3(0.0f));

    glm::vec3 length = glm::normalize(glm::vec3(cos, sin, 0) - glm::vec3(0, 0, m_height));
    glm::vec3 normal = glm::normalize(glm::cross(up, length)); //points towards user
    return glm::normalize(glm::cross(length, normal)); //faces away from cone
}

void Cone::init() {
    if(m_vao != 0) return;

    std::vector<GLfloat> points;
    std::vector<GLfloat> normals;
    std::vector<GLuint> elements;

    for(unsigned int x = 0; x < m_slices; ++x) {
        // points
        GLfloat theta0 = glm::two_pi<GLfloat>() * (GLfloat)x / (GLfloat)m_slices;
        GLfloat theta1 = glm::two_pi<GLfloat>() * (GLfloat)(x+1) / (GLfloat)m_slices;
        theta1 = (theta0 + theta1) / 2; // middle part
        GLfloat cos0 = (GLfloat)m_radius * glm::cos(theta0);
        GLfloat cos1 = (GLfloat)m_radius * glm::cos(theta1);
        GLfloat sin0 = (GLfloat)m_radius * glm::sin(theta0);
        GLfloat sin1 = (GLfloat)m_radius * glm::sin(theta1);
        PUSH_BACK3(points, cos0, sin0, 0.0f);
        PUSH_BACK3(points, 0.0f, 0.0f, m_height);

        // normals
        glm::vec3 normal = calculateNormal(sin0, cos0);
        PUSH_BACK3(normals, normal.x, normal.y, normal.z);

        normal = calculateNormal(sin1, cos1);
        PUSH_BACK3(normals, normal.x, normal.y, normal.z);

        // elements
        PUSH_BACK3(elements, x * 2, ((x + 1) % m_slices) * 2, (x * 2) + 1);
    }

    TriangleMesh::init(&elements, &points, &normals);
}
