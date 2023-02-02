#pragma once

#include "renderer.h"

class VertexBuffer
{
    public:
    VertexBuffer(unsigned int size);
    VertexBuffer(const void * data, unsigned int size);
    ~VertexBuffer();
    void Bind() const;
    void Unbind() const;

    private:
    GLuint m_RendererID;
};