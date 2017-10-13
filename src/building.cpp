#include "shapes.h"

Building::Building(glm::vec3 b[], float h[]) {
    for(int x = 0; x < 4; ++x) {
        base[x] = b[3 - x];
        height[x] = h[3 - x];
    }
}

void Building::init() {
    if (m_vao != 0) return;

	std::vector<GLfloat> pts;
	std::vector<GLfloat> norm;
	std::vector<GLuint> el;

	// Side faces
	buildFace(pts, norm, el, 0, 1);
	buildFace(pts, norm, el, 1, 2);
	buildFace(pts, norm, el, 2, 3);
	buildFace(pts, norm, el, 3, 0);

	// Top
	buildTopTri(pts, norm, el, 0, 1, 2);
	buildTopTri(pts, norm, el, 0, 2, 3);

	TriangleMesh::init(&el, &pts, &norm);
}

void Building::buildFace(std::vector<GLfloat>& pts, std::vector<GLfloat>& norm,
                         std::vector<GLuint>& el, int idx1, int idx2) {
	GLuint start = pts.size() / 3;
	glm::vec3 p[4];
	p[0] = base[idx1];
	p[1] = base[idx2];
	p[2] = glm::vec3(p[1].x, height[idx2], p[1].z);
	p[3] = glm::vec3(p[0].x, height[idx1], p[0].z);
	glm::vec3 n = glm::cross(p[1] - p[0], p[2] - p[0]);
	n = glm::normalize(n);

	for(unsigned int i = 0; i < 4; i++) {
        PUSH_BACK_VEC3(pts, p[i]);
		PUSH_BACK_VEC3(norm, n);
	}
    PUSH_BACK3(el, start + 0, start + 1, start + 2);
    PUSH_BACK3(el, start + 0, start + 2, start + 3);
}

void Building::buildTopTri(std::vector<GLfloat>& pts, std::vector<GLfloat>& norm,
	                       std::vector<GLuint>& el, int idx1, int idx2, int idx3) {
	GLuint start = pts.size() / 3;
	glm::vec3 p[3];
	p[0] = glm::vec3(base[idx1].x, height[idx1], base[idx1].z);
	p[1] = glm::vec3(base[idx2].x, height[idx2], base[idx2].z);
	p[2] = glm::vec3(base[idx3].x, height[idx3], base[idx3].z);
	glm::vec3 n = glm::cross(p[1] - p[0], p[2] - p[0]);
	n = glm::normalize(n);

	for(unsigned int i = 0; i < 3; i++) {
        PUSH_BACK_VEC3(pts, p[i]);
		PUSH_BACK_VEC3(norm, n);
	}
    PUSH_BACK3(el, start + 0, start + 1, start + 2);
}
