#include "mesh.h"

using std::vector;

void TriangleMesh::render() {
    if(m_vao == 0) throw std::runtime_error("Cannot render uninitlized TriangleMesh.");
    QOpenGLFunctions_4_1_Core* gl =
            QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

    // Bind the VAO.  This re-enables the settings stored in the VAO including
    // the connections between vertex attributes (shader inputs) and vertex buffers.
    gl->glBindVertexArray(m_vao);
    // Draw the triangles using the buffers defined in the VAO
    gl->glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_INT, 0);
    // Un-bind the VAO
    gl->glBindVertexArray(0);
}

void TriangleMesh::init(vector<GLuint>*  tris,        // The index data
                        vector<GLfloat>* points,      // The position data (must be non-NULL)
                        vector<GLfloat>* normals,     // The normal vector data (can be NULL)
                        vector<GLfloat>* colors,      // The color data (can be NULL)
                        vector<GLfloat>* texCoords) { // The texture coordinate data (can be NULL)
    if( tris == NULL || points == NULL )
        qFatal("initGpuVertexArrays: the index data and position data must be non-NULL.");

    // Store the number of elements for later rendering.
    m_elements = tris->size();

    QOpenGLFunctions_4_1_Core* gl =
            QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_1_Core>();

    // Determine the number of buffers needed
    int nBuffers = 2;
    if( normals != NULL ) nBuffers++;
    if( colors != NULL ) nBuffers++;
    if( texCoords != NULL ) nBuffers++;

    // Generate the OpenGL buffers
    m_buffers.clear();
    m_buffers.insert(m_buffers.end(), nBuffers, 0);
    gl->glGenBuffers(nBuffers, &m_buffers[0]);

    // Copy data into each buffer.  This is done by first binding the buffer to
    // the GL_ARRAY_BUFFER binding point, and then copying the data into the
    // buffer that is bound to that point using glBufferData.

    int bufIndex = 0;
    // Copy data into the index buffer
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
    gl->glBufferData(GL_ARRAY_BUFFER, tris->size() * sizeof(GLuint), &(*tris)[0], GL_STATIC_DRAW);

    // Copy data into the point buffer
    bufIndex++;
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
    gl->glBufferData(GL_ARRAY_BUFFER, points->size() * sizeof(GLfloat), &(*points)[0], GL_STATIC_DRAW);

    if( normals != NULL ) {
        // Copy data into the normals buffer
        bufIndex++;
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
        gl->glBufferData(GL_ARRAY_BUFFER, normals->size() * sizeof(GLfloat), &(*normals)[0], GL_STATIC_DRAW);
    }

    if( colors != NULL ) {
        // Copy into the colors buffer
        bufIndex++;
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
        gl->glBufferData(GL_ARRAY_BUFFER, colors->size() * sizeof(GLfloat), &(*colors)[0], GL_STATIC_DRAW);
    }

    if( texCoords != NULL ) {
        // Copy into the tex coordinate buffer
        bufIndex++;
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
        gl->glBufferData(GL_ARRAY_BUFFER, texCoords->size() * sizeof(GLfloat), &(*texCoords)[0], GL_STATIC_DRAW);
    }

    // Create and set up the vertex array object (VAO).  The VAO contains
    // settings that determine connections between the buffers and shader
    // inputs.  The glVertexAttribPointer sets up a link between a shader vertex
    // attribute and the buffer that contains the data for that attribute.  That
    // link is stored in the VAO that is currently bound (glBindVertexArray).
    // When a VAO is re-bound at some later point (using glBindVertexArray) all
    // of the settings for that VAO are re-enabled.  When a render command is
    // issued (such as glDrawElements or glDrawArrays), vertex data (position,
    // normal, etc.) is retrieved from the appropriate buffer, determined by the
    // VAO that is bound.

    // Create the VAO
    gl->glGenVertexArrays(1, &m_vao);
    // Bind the newly created VAO
    gl->glBindVertexArray(m_vao);

    // Set up the element buffer.  The buffer bound to GL_ELEMENT_ARRAY_BUFFER
    // is used by glDrawElements to pull index data (the indices used to access
    // data in the other buffers).
    bufIndex = 0;
    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[bufIndex]);

    // Set up the point (position) pointer.  The position is bound to vertex
    // attribute 1.
    bufIndex++;
    gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    gl->glEnableVertexAttribArray(1);

    if( normals != NULL ) {
        // Set up the normal pointer.  The normal data is bound to vertex attribute 2.
        bufIndex++;
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
        gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
        gl->glEnableVertexAttribArray(2);
    }

    if( colors != NULL ) {
        // Set up the color pointer.  Color data is bound to vertex attribute 3.
        bufIndex++;
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
        gl->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
        gl->glEnableVertexAttribArray(3);
    }

    if( texCoords != NULL ) {
        // Set up the tex-coordinate pointer. Texture coordinates are bound to
        // vertex attribute 4.
        bufIndex++;
        gl->glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufIndex]);
        gl->glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, 0);
        gl->glEnableVertexAttribArray(4);
    }

    // Unbind the VAO
    gl->glBindVertexArray(0);
}
