#pragma once

#include "includes/opengl/VertexArray.h"
#include "includes/opengl/VertexBuffer.h"
#include "includes/opengl/IndexBuffer.h"

class Quad
{
public:
    Quad();

    unsigned int IndexCount() { return 6; }
    
    void Bind();
    void BindVB();

    void UpdateBuffer(glm::vec2 pos, glm::vec2 size, glm::vec2 texturePos, glm::vec2 textureSize);

private:

    VertexArray m_VertexArray;
    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    constexpr static const unsigned int quadIndices[6] = {
        0,1,2,
        0,2,3
    };
};

