#pragma once

#include <QJsonObject>
#include <glm/glm.hpp>

#include "mesh.h"

struct Circular {
protected:
    GLfloat m_radius;
    GLuint m_slices;

public:
    Circular(GLfloat r, GLuint s) : m_radius(r), m_slices(s) {}
    virtual ~Circular() {}
    virtual GLfloat getRadius() { return m_radius; }
    virtual GLuint  getSlices() { return m_slices; }
};



class Cube : public TriangleMesh {
protected:
    GLfloat m_side;
public:
    Cube(GLfloat side) : m_side(side) {}
    virtual ~Cube() {}
    GLfloat getSide() { return m_side; }
    virtual void init();
};


class Disk : public TriangleMesh, public Circular {
public:
    Disk(GLfloat r, GLuint s) : Circular(r, s) {}
    virtual ~Disk() {}
    virtual void init();
};


class Cone : public TriangleMesh, public Circular {
    GLfloat m_height;
    glm::vec3 calculateNormal(GLfloat sin, GLfloat cos);
public:
    Cone(GLfloat r, GLfloat h, GLuint s) : Circular(r, s), m_height(h) {}
    virtual ~Cone() {}
    virtual GLfloat getHeight() { return m_height; }
    virtual void init();
};


class Cylinder : public TriangleMesh, public Circular {
    GLfloat m_height;
public:
    Cylinder(GLfloat r, GLfloat h, GLuint s) : Circular(r, s), m_height(h) {}
    virtual ~Cylinder() {}
    virtual GLfloat getHeight() { return m_height; }
    virtual void init();
};


/// A simple mesh consisting of two triangles
class Quad : public TriangleMesh {
    /// The four corners of the mesh
    glm::vec3 p[4];
public:
    /// @param p An array of the four mesh corners
	Quad(glm::vec3 p[4]) { setPosition(p); }
    virtual ~Quad() {}
    virtual void init();

    /// @param np An array of the four new mesh corners
	virtual inline void setPosition(glm::vec3 np[4]) {
        // Not sure why, but needed for points to be accepted in counter-clockwise order
        for(int x = 0; x < 4; ++x) p[3 - x] = np[x];
    }
};


class Track : public TriangleMesh {
    std::vector<GLfloat> left_curb;
    std::vector<GLfloat> right_curb;
    /// Normal map texture id
    GLuint normal_map_id;

public:
    Track(QJsonObject a);
    virtual ~Track() {}
    virtual void init();
    virtual void setUniform(Shader& ) {
        QOpenGLFunctions_4_1_Core* gl =
          QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();
        //s.setUniform("normal_map", 0);
        gl->glActiveTexture(GL_TEXTURE0);
        gl->glBindTexture(GL_TEXTURE_2D, normal_map_id);
    }
};

// TODO: Update this to be more capable
class Building : public TriangleMesh {
    /// The four base points
	glm::vec3 base[4];
	/// The height of the building at each base point.
	float height[4];

    void buildFace(std::vector<GLfloat>& pts, std::vector<GLfloat>& norm,
		std::vector<GLuint>& el, int idx1, int idx2);
	void buildTopTri(std::vector<GLfloat>& pts, std::vector<GLfloat>& norm,
		std::vector<GLuint>& el, int idx1, int idx2, int idx3);

public:
    /**
     * @param b An array containing four points that describe the base of the
     * building in counter-clockwise order.
     * @param h An array of size four containing the height of the building above
     * each of the four points in b.
     */
    Building(glm::vec3 b[4], float h[4]);
    virtual ~Building() {}
    virtual void init();
};


class CustomTriangleMesh : public TriangleMesh {
    std::vector<GLuint>*  tris;        // The index data
    std::vector<GLfloat>* points;      // The position data (must be non-NULL)
    std::vector<GLfloat>* normals;     // The normal vector data (can be NULL)
    std::vector<GLfloat>* colors;      // The color data (can be NULL)
    std::vector<GLfloat>* texcoords;   // The texture coordinate data (can be NULL)

public:
    CustomTriangleMesh(std::vector<GLuint>* tris,
                       std::vector<GLfloat>* points,
                       std::vector<GLfloat>* normals   = nullptr,
                       std::vector<GLfloat>* colors    = nullptr,
                       std::vector<GLfloat>* texcoords = nullptr) :
           tris(tris), points(points), normals(normals), colors(colors),
           texcoords(texcoords) {}

    virtual ~CustomTriangleMesh() {
        if(tris != nullptr)      delete tris;
        if(points != nullptr)    delete points;
        if(normals != nullptr)   delete normals;
        if(colors != nullptr)    delete colors;
        if(texcoords != nullptr) delete texcoords;
    }
    virtual void init() { TriangleMesh::init(tris, points, normals, colors, texcoords); }
};
