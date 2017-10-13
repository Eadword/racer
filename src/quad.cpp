#include "shapes.h"

void Quad::init() {
    if (m_vao != 0) return;

    std::vector<GLfloat> pts;
    std::vector<GLfloat> norm;
    std::vector<GLuint> el = { 0, 1, 2, 0, 2, 3 };

    glm::vec3 n = glm::normalize(glm::cross(p[1] - p[0], p[2] - p[0]));

    for(int i = 0; i < 4; i++) {
        PUSH_BACK_VEC3(pts, p[i]);
        PUSH_BACK_VEC3(norm, n);
    }

    TriangleMesh::init(&el, &pts, &norm);
}
