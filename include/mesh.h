#pragma once

#include <QOpenGLFunctions_4_1_Core>
#include <vector>
#include "shader.h"

#define PUSH_BACK3(vector, a, b, c) {   \
    vector.push_back(a);                \
    vector.push_back(b);                \
    vector.push_back(c);                \
}

#define PUSH_BACK_VEC3(vector, v3) PUSH_BACK3(vector, v3.x, v3.y, v3.z)


class Mesh {
protected:
    /// The ID of the Vertex Array Object (VAO).
    GLuint m_vao;

    /// The IDs of the OpenGL buffers
    std::vector<GLuint> m_buffers;

    /// The number of elements in the index data. For example, if there are 3 triangles,
    /// they must be described by 9 indices, so this value would be 9.
    GLuint m_elements;

public:
    Mesh() : m_vao(0), m_elements(0) {}
    virtual ~Mesh() { destroy(); }

    /**
    * Deletes and frees all OpenGL buffers and VAO associated with this mesh.
    * Make sure a valid OpenGL context is current before calling this method.
    */
    virtual void destroy();
    virtual void init() = 0;
    virtual void render() = 0;
    virtual void setUniform(Shader&) {}

    virtual inline bool isInit() { return m_vao != 0;}
};


/**
 * An abstract class representing an object with only triangles.
 * Call init once OpenGL is ready, and then render as desired.
 */
class TriangleMesh : public Mesh {
protected:
    /**
     * Copies the vertex data to OpenGL GPU buffers and sets up a VAO for using
     * the buffers. Points normals, and texCoords must have same number of verts
     * if not null.
     *
     * @param triangles Indices describing the faces of the mesh. Each index referes to first of vertex, so index 2 would be at 6 in the array.
     * @param points    Position data, assumed to be stored in [x0, y0, z0, x1, y1, z1, ...] format.
     * @param normals   Stored in same way as points, except vectors representing normals for each point (must be same number of normals as points).
     * @param colors    Stored in floating point rbga format. Again, must have same number of color sets as points.
     * @param texCoords texture-coordinate data. Assumed to be 2-floats per vertex [s0, t0, s1, t1, ...]
     */
    virtual void init(
        std::vector<GLuint>*   triangles,
        std::vector<GLfloat>*  points,
        std::vector<GLfloat>*  normals,
        std::vector<GLfloat>*  colors    = NULL,
        std::vector<GLfloat>*  texCoords = NULL
    );

public:
    /// Creates an empty TriangleMesh
    TriangleMesh() {}

    /// Deletes all of the triangle data in OpenGL memory (calls destroy())
    virtual ~TriangleMesh() { destroy(); }

    /**
     * Constructs the data based on a mathematical shape. The implimenting child
     * needs to then call the other init to initlize the GPU buffers.
     */
    virtual void init() = 0;

    virtual void render();
};
