#include <glm/gtc/constants.hpp>

#include "shapes.h"

void Cylinder::init() {
    if(m_vao != 0) return;

    std::vector<GLfloat> points;
    std::vector<GLfloat> normals;
    std::vector<GLuint> elements;

    for(unsigned int x = 0; x < m_slices; ++x) {
        // points
        GLfloat theta0 = glm::two_pi<GLfloat>() * (GLfloat)x / (GLfloat)m_slices;
        GLfloat cos0 = (GLfloat)m_radius * glm::cos(theta0);
        GLfloat sin0 = (GLfloat)m_radius * glm::sin(theta0);
        PUSH_BACK3(points, cos0, sin0, m_height);
        PUSH_BACK3(points, cos0, sin0, 0.0f);

        // normals
        glm::vec3 normal(cos0, sin0, 0.0f);
        normal = glm::normalize(normal - glm::vec3(0.0f));
        PUSH_BACK_VEC3(normals, normal);
        PUSH_BACK_VEC3(normals, normal);

        // elements
        GLuint s0 = x * 2;
        GLuint s1 = ((x + 1) % m_slices) * 2;     // 0——2 ...
        PUSH_BACK3(elements, s0, s0 + 1, s1 + 1); // |  |
        PUSH_BACK3(elements, s1 + 1, s1, s0);     // 1——3 ...
    }

    TriangleMesh::init(&elements, &points, &normals);
}
