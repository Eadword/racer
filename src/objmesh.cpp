#include "objmesh.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

ObjMesh::ObjMesh(const std::string & fName, Shader& s ) : fileName(fName), shader(s) { }

ObjMesh::~ObjMesh() {}

void ObjMesh::init()
{
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> mats;

  std::string err;
  bool ret = tinyobj::LoadObj(shapes, mats, err, fileName.c_str());

  if (!err.empty()) { // `err` may contain warning message.
    std::cerr << err << std::endl;
  }

  if (!ret) {
    qFatal("Unable to load: %s", fileName.c_str());
  }

  for (size_t i = 0; i < mats.size(); i++) {
    Material m;
    m.Ka = glm::vec3(mats[i].ambient[0], mats[i].ambient[1], mats[i].ambient[2]);
    m.Kd = glm::vec3(mats[i].diffuse[0], mats[i].diffuse[1], mats[i].diffuse[2]);
    m.Ks = glm::vec3(mats[i].specular[0], mats[i].specular[1], mats[i].specular[2]);
    m.shine = mats[i].shininess;
    materials.push_back(m);
  }

  std::vector<GLfloat> pts;
  std::vector<GLfloat> norm;
  std::vector<GLuint> el;

  glm::vec3
    min(std::numeric_limits<float>::max()),
    max(std::numeric_limits<float>::min());

  for (size_t i = 0; i < shapes.size(); i++) {
    tinyobj::mesh_t & m = shapes[i].mesh;

    GLuint startIndex = pts.size() / 3;
    for (size_t v = 0; v < m.positions.size() / 3; v++) {
      GLfloat x = m.positions[3*v+0];
      GLfloat y = m.positions[3*v+1];
      GLfloat z = m.positions[3*v+2];

      if( min.x > x ) min.x = x;
      if( min.y > y ) min.y = y;
      if( min.z > z ) min.z = z;
      if( max.x < x ) max.x = x;
      if( max.y < y ) max.y = y;
      if( max.z < z ) max.z = z;

      pts.push_back(x);
      pts.push_back(y);
      pts.push_back(z);
    }

    ObjShape s;
    if(shapes.size() > 0)
    {
      s.nVerts = 0;
      s.start = el.size();
      s.matIndex = m.material_ids[0];
    }
    for (size_t f = 0; f < m.indices.size() / 3; f++) {
      GLuint matId = m.material_ids[f];
      if( s.matIndex != matId )
      {
        if(s.nVerts > 0) parts.push_back(s);
        s.nVerts = 0;
        s.start = el.size();
        s.matIndex = matId;
      }
      el.push_back(m.indices[3*f+0] + startIndex);
      el.push_back(m.indices[3*f+1] + startIndex);
      el.push_back(m.indices[3*f+2] + startIndex);
      s.nVerts += 3;
    }
    if( s.nVerts > 0 ) parts.push_back(s);
  }

  printf("OBJ: %s\n", fileName.c_str());
  printf("  BBox:   (%.4f, %.4f, %.4f) -> (%.4f, %.4f, %.4f)\n", min.x, min.y, min.z, max.x, max.y, max.z);
  glm::vec3 c = 0.5f * (min + max);
  printf("  Center: (%.4f, %.4f, %.4f)\n", c.x, c.y, c.z);

  generateNormals(pts, norm, el);

  TriangleMesh::init(&el, &pts, &norm);
}

void ObjMesh::generateNormals( std::vector<GLfloat> &pts, std::vector<GLfloat> &norm, std::vector<GLuint> &faces)
{
  norm.clear();
  norm.reserve(pts.size());
  norm.insert( norm.end(), pts.size(), 0.0f);  // Fill with zeros

  for( GLuint i = 0; i < parts.size(); i++ )
  {
    ObjShape s = parts[i];
    glm::vec3 p0, p1, p2, n;
    for( GLuint i = s.start; i < s.start + s.nVerts; i += 3 )
    {
      p0 = glm::vec3( pts[3*faces[i+0]+0], pts[3*faces[i+0]+1], pts[3*faces[i+0]+2] );
      p1 = glm::vec3( pts[3*faces[i+1]+0], pts[3*faces[i+1]+1], pts[3*faces[i+1]+2] );
      p2 = glm::vec3( pts[3*faces[i+2]+0], pts[3*faces[i+2]+1], pts[3*faces[i+2]+2] );

      n = glm::normalize( glm::cross(p1 - p0, p2 - p0) );
      for( GLuint j = 0; j < 3; j++ )
      {
        norm[3*faces[i+j]+0] += n.x;
        norm[3*faces[i+j]+1] += n.y;
        norm[3*faces[i+j]+2] += n.z;
      }
    }
  }

  glm::vec3 n;
  for(GLuint i = 0; i < norm.size(); i+=3)
  {
    n.x = norm[i+0];
    n.y = norm[i+1];
    n.z = norm[i+2];
    n = glm::normalize(n);
    norm[i+0] = n.x;
    norm[i+1] = n.y;
    norm[i+2] = n.z;
  }
}

void ObjMesh::render() {
    if( m_vao == 0 ) throw std::runtime_error("Cannot render uninitlized ObjMesh.");

    QOpenGLFunctions_4_1_Core* gl =
      QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

    // Bind the VAO.  This re-enables the settings stored in the VAO including
    // the connections between vertex attributes (shader inputs) and vertex buffers.
    gl->glBindVertexArray(m_vao);
    for( GLuint i = 0; i < parts.size(); i++ )
    {
      materials[ parts[i].matIndex ].setUniforms(shader);
      // Draw the triangles using the buffers defined in the VAO
      gl->glDrawElements(GL_TRIANGLES, parts[i].nVerts, GL_UNSIGNED_INT, (GLvoid *)(sizeof(GLuint) * parts[i].start));
    }
    // Un-bind the VAO
    gl->glBindVertexArray(0);
}
