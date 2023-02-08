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

void Cube::Bind() const
{
    m_VertexArray.Bind();
}

void Cube::BindVB() const
{
    m_VertexBuffer.Bind();
}

void Cube::UpdateBuffer(glm::vec3 pos, glm::vec3 size, glm::vec2 texturePos, glm::vec2 textureSize, const Model& m)
{
    float vertices[36][9] = { // cube between 0 and 1
        // face 1
        { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.front}, //0
        { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.front}, //1
        { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.front}, //2
        { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.front}, //0
        { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.front}, //2
        { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.front}, //4
        // face 2 (bottom)
        { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, (float)m.bottom}, //5
        { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, (float)m.bottom}, //0
        { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, (float)m.bottom}, //6
        { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, (float)m.bottom}, //5
        { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, (float)m.bottom}, //1
        { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, (float)m.bottom}, //0
        // face 3
        { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.left}, //3
        { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.left}, //6
        { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.left}, //0
        { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.left}, //3
        { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.left}, //0
        { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.left}, //4
        // face 4
        { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.right}, //7
        { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.right}, //6
        { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.right}, //3
        { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.right}, //6
        { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.right}, //7
        { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.right}, //5
        // face 5
        { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.back}, //2
        { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.back}, //1
        { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.back}, //5
        { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.back}, //7
        { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.back}, //2
        { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.back}, //5
        // face 6 (top)
        { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.top}, //7
        { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.top}, //3
        { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.top}, //4
        { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.top}, //7
        { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.top}, //4
        { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.top}, //2
    };
    
    BindVB();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 9 * 36, vertices));
}


    // { 0.0f, 0.0f, 0.0f,  1.0f, 0.0f}, //0
    // { 0.0f, 0.0f, 1.0f,  0.0f, 1.0f}, //1
    // { 0.0f, 1.0f, 1.0f,  0.0f, 0.0f}, //2
    // { 1.0f, 1.0f, 0.0f,  1.0f, 1.0f}, //3
    // { 0.0f, 1.0f, 0.0f,  0.0f, 1.0f}, //4
    // { 1.0f, 0.0f, 1.0f,  1.0f, 0.0f}, //5
    // { 1.0f, 0.0f, 0.0f,  0.5f, 1.0f}, //6
    // { 1.0f, 1.0f, 1.0f,  1.0f, 0.5f}, //7