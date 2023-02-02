#include "quad.h"

#include "../opengl/VertexBufferLayout.h"

Quad::Quad()
    : m_IndexBuffer(quadIndices, 6), m_VertexBuffer(4)
{
    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3);
    m_VertexArray.AddBuffer(m_VertexBuffer, layout);
    m_IndexBuffer.Bind();
}

void Quad::UpdateVertices(glm::vec2 pos, glm::vec2 size)
{
    m_VertexArray.Bind();
    float vertices[4][2] = {
        { pos.x,          pos.y + size.y }, // 0            
        { pos.x,          pos.y,         }, // 1
        { pos.x + size.x, pos.y,         }, // 2
        { pos.x + size.x, pos.y + size.y }  // 3
    };

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * 2, vertices);
}