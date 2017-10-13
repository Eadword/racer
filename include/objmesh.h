#pragma once

#include <string>

#include "material.h"
#include "mesh.h"
#include "shader.h"

class ObjMesh : public TriangleMesh {
    struct ObjShape {
        GLuint nVerts;
        GLuint start;
        GLuint matIndex;
    };

    std::string fileName;
    std::vector<Material> materials;
    std::vector<ObjShape> parts;

    Shader& shader;

    void generateNormals( std::vector<GLfloat> &pts, std::vector<GLfloat> &norm, std::vector<GLuint> &faces);

public:
    ObjMesh(const std::string & fName, Shader& s);
    virtual ~ObjMesh();

    virtual void init();
    virtual void render();
};
