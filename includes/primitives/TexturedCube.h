#pragma once

#include "../opengl/VertexArray.h"
#include "../opengl/VertexBuffer.h"
#include "../opengl/IndexBuffer.h"

class Cube
{
public:
    Cube();

    unsigned int IndexCount() { return 36; }
    
    void Bind();
    void BindVB();

    void UpdateBuffer(glm::vec2 pos, glm::vec2 size, glm::vec2 texturePos, glm::vec2 textureSize);

private:

    VertexArray m_VertexArray;
    VertexBuffer m_VertexBuffer;

};

