#include "TexturedCube.h"

#include "../opengl/VertexBufferLayout.h"

Cube::Cube()
    : m_VertexArray(), m_VertexBuffer(36*9*sizeof(float))
{
    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    m_VertexArray.AddBuffer(m_VertexBuffer, layout);
}

void Cube::Bind()
{
    m_VertexArray.Bind();
}

void Cube::BindVB()
{
    m_VertexBuffer.Bind();
}

void Cube::UpdateBuffer(glm::vec2 pos, glm::vec2 size, glm::vec2 texturePos, glm::vec2 textureSize)
{
    float vertices[36][9] = { // cube between 0 and 1
        // face 1
        { 0.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //0
        { 0.0f, 0.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, 1.0f}, //1
        { 0.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //2
        { 0.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //0
        { 0.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //2
        { 0.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, 1.0f}, //4
        // face 2 (bottom)
        { 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f}, //5
        { 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f}, //0
        { 1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f}, //6
        { 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f}, //5
        { 0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f}, //1
        { 0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f}, //0
        // face 3
        { 1.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //3
        { 1.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, 1.0f}, //6
        { 0.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //0
        { 1.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //3
        { 0.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //0
        { 0.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, 1.0f}, //4
        // face 4
        { 1.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //7
        { 1.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //6
        { 1.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, 1.0f}, //3
        { 1.0f, 0.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //6
        { 1.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //7
        { 1.0f, 0.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, 1.0f}, //5
        // face 5
        { 0.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, 1.0f}, //2
        { 0.0f, 0.0f, 1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 1.0f}, //1
        { 1.0f, 0.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, 1.0f}, //5
        { 1.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 1.0f}, //7
        { 0.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, 1.0f}, //2
        { 1.0f, 0.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, 1.0f}, //5
        // face 6 (top)
        { 1.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 2.0f}, //7
        { 1.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, 2.0f}, //3
        { 0.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 2.0f}, //4
        { 1.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, 2.0f}, //7
        { 0.0f, 1.0f, 0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, 2.0f}, //4
        { 0.0f, 1.0f, 1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, 2.0f}, //2
    };
    
    // { 0.0f, 0.0f, 0.0f,  1.0f, 0.0f}, //0
    // { 0.0f, 0.0f, 1.0f,  0.0f, 1.0f}, //1
    // { 0.0f, 1.0f, 1.0f,  0.0f, 0.0f}, //2
    // { 1.0f, 1.0f, 0.0f,  1.0f, 1.0f}, //3
    // { 0.0f, 1.0f, 0.0f,  0.0f, 1.0f}, //4
    // { 1.0f, 0.0f, 1.0f,  1.0f, 0.0f}, //5
    // { 1.0f, 0.0f, 0.0f,  0.5f, 1.0f}, //6
    // { 1.0f, 1.0f, 1.0f,  1.0f, 0.5f}, //7

    BindVB();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 9 * 36, vertices));
}