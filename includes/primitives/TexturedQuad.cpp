#include "TexturedQuad.h"

#include "../opengl/VertexBufferLayout.h"

Quad::Quad()
    : m_VertexArray(), m_IndexBuffer(quadIndices, 3*2), m_VertexBuffer(std::make_shared<VertexBuffer>(4*4*sizeof(float)))
{
    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 2);
    layout.Push(GL_FLOAT, 2);
    m_VertexArray.AddBuffer(m_VertexBuffer, layout);
    m_IndexBuffer.Bind();
}

void Quad::Bind()
{
    m_VertexArray.Bind();
}

void Quad::BindVB()
{
    m_VertexBuffer->Bind();
}

void Quad::UpdateBuffer(glm::vec2 pos, glm::vec2 size, glm::vec2 texturePos, glm::vec2 textureSize)
{
    float vertices[4][4] = {
        { pos.x,          pos.y + size.y, texturePos.x,               texturePos.y}, // 0            
        { pos.x,          pos.y,          texturePos.x,               texturePos.y+textureSize.y}, // 1
        { pos.x + size.x, pos.y,          texturePos.x+textureSize.x, texturePos.y+textureSize.y}, // 2
        { pos.x + size.x, pos.y + size.y, texturePos.x+textureSize.x, texturePos.y}  // 3
    };
    BindVB();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * 4, vertices));
}