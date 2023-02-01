#pragma once

#include "../opengl/Renderer.h"
#include "../opengl/VertexArray.h"
#include "../opengl/IndexBuffer.h"

class Quad
{
public:
    Quad();

private:

    VertexArray m_VertexArray;
    IndexBuffer m_IndexBuffer;
    

    constexpr static const unsigned int quadIndices[6] = {
        0,1,2,
        0,2,3
    };
};

