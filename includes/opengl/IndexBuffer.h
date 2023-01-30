#pragma once

#include "renderer.h"

class IndexBuffer
{
    public:
    IndexBuffer(const GLuint * data, unsigned int count);
    ~IndexBuffer();
    void Bind() const;
    void Unbind() const;

    private:
    GLuint m_BufferID;
    GLuint m_Count;
};