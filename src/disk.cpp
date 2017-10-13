#include <glm/gtc/constants.hpp>

#include "shapes.h"

void Disk::init() {
    if(m_vao != 0) return;

    std::vector<GLfloat> points;
    std::vector<GLfloat> normals;
    std::vector<GLuint> elements;

    for(unsigned int x = 0; x < m_slices; ++x) {
        // points
        GLfloat theta0 = glm::two_pi<GLfloat>() * (GLfloat)x / (GLfloat)m_slices;
        GLfloat cos0 = (GLfloat)m_radius * glm::cos(theta0);
        GLfloat sin0 = (GLfloat)m_radius * glm::sin(theta0);
        PUSH_BACK3(points, cos0, sin0, 0.0f);

        // normals
        PUSH_BACK3(normals, 0.0f, 0.0f, 1.0f);

        // elements
        PUSH_BACK3(elements, x, (x + 1) % m_slices, m_slices);
    }
    PUSH_BACK3(points, 0.0f, 0.0f, 0.0f);
    PUSH_BACK3(normals, 0.0f, 0.0f, 1.0f);

    TriangleMesh::init(&elements, &points, &normals);
}
