#include <QJsonArray>
#include <QJsonValue>
#include "shapes.h"
#include "procedural.h"

Track::Track(QJsonObject a) : normal_map_id(0) {
    if(!a.contains("leftCurb") || !a.contains("rightCurb"))
        throw std::invalid_argument("Track did not recieve a QJsonObject with left and right curb.");
    if(!a["leftCurb"].isArray() || !a["rightCurb"].isArray())
        throw std::invalid_argument("Track did not recieve a QJsonObject with curb arrays.");
    QJsonArray lcurb = a["leftCurb"].toArray();
    QJsonArray rcurb = a["rightCurb"].toArray();

    for(auto&& i : lcurb) {
        if(!i.isDouble()) throw std::invalid_argument("Track found invalid data within left curb.");
        left_curb.push_back((GLfloat)i.toDouble());
    }
    for(auto&& i : rcurb) {
        if(!i.isDouble()) throw std::invalid_argument("Track found invalid data within right curb.");
        right_curb.push_back((GLfloat)i.toDouble());
    }
}

void Track::init() {
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    // how many times it repeats the normal map for a 1x1 world coordinate box
    const float repeate_rate = 0.25f;

    if(m_vao != 0) return;

    std::vector<GLfloat> points;
    std::vector<GLfloat> normals;
    std::vector<GLuint> elements;
    std::vector<GLfloat> uvcoords;

    // Just in case the data is not as we expect
    unsigned int max = std::min(left_curb.size(), right_curb.size()) / 3;

    // Add to the vectors
    for(unsigned int x = 0; x < max; ++x) {
        // points
        PUSH_BACK3(points, left_curb[x*3], left_curb[x*3 + 1], left_curb[x*3 + 2]);
        PUSH_BACK3(points, right_curb[x*3], right_curb[x*3 + 1], right_curb[x*3 + 2]);

        //UV Mapping
        uvcoords.push_back(left_curb[x*3] * repeate_rate);
        uvcoords.push_back(left_curb[x*3 + 2] * repeate_rate);
        uvcoords.push_back(right_curb[x*3] * repeate_rate);
        uvcoords.push_back(right_curb[x*3 + 2] * repeate_rate);

        // normals
        PUSH_BACK_VEC3(normals, up);
        PUSH_BACK_VEC3(normals, up);
    }
    for(unsigned int x = 0; x < max*2; x+=2) {
        // elements
        unsigned int n = (x + 2) % (max*2); //the next left curb val
        PUSH_BACK3(elements, x, x + 1, n + 1);  // left:  0 -- 2 -- 4
        PUSH_BACK3(elements, n + 1, n, x);      // right: 1 -- 3 -- 5
    }

    TriangleMesh::init(&elements, &points, &normals, nullptr, &uvcoords);


    //Generate normal map
    const uint32_t size = 512;
    GLubyte* height_map = Procedural::generateHeightMap(size, size);
    GLubyte* normal_map = Procedural::generateNormalMap(size, size, height_map); //Procedural::heightMapToRGBA(size, size, height_map);
    delete[] height_map;

    QOpenGLFunctions_4_1_Core* gl =
  		QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

    gl->glActiveTexture(GL_TEXTURE0);
    gl->glGenTextures(1, &normal_map_id);

    gl->glBindTexture(GL_TEXTURE_2D, normal_map_id);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, &normal_map[0]);
}
