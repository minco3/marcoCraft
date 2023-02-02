#pragma once

#include "../opengl/VertexArray.h"
#include "../opengl/VertexBuffer.h"
#include "../opengl/IndexBuffer.h"

class Quad
{
public:
    Quad();

    VertexArray& GetVertexArray() { return m_VertexArray; }
    unsigned int IndexCount() { return 6; }

    void UpdateVertices(glm::vec2 pos, glm::vec2 size);

private:

    VertexArray m_VertexArray;
    VertexBuffer m_VertexBuffer;
    IndexBuffer m_IndexBuffer;

    constexpr static const unsigned int quadIndices[6] = {
        0,1,2,
        0,2,3
    };
};

