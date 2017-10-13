#include "mesh.h"

void Mesh::destroy() {
    if( m_vao == 0 ) return;
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if(ctx == nullptr) return;

    QOpenGLFunctions_4_1_Core* gl = ctx->versionFunctions<QOpenGLFunctions_4_1_Core>();

    if(gl == nullptr) return;
    //qDebug("Deleting vertex buffers...");
    gl->glDeleteBuffers(m_buffers.size(), &m_buffers[0]);
    m_buffers.clear();
    gl->glDeleteVertexArrays(1, &m_vao);
    m_vao = 0;
}
