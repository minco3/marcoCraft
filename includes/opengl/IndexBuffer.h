#pragma once

#include "Renderer.h"

class IndexBuffer
{
public:
    IndexBuffer(const GLuint * data, unsigned int count);
    ~IndexBuffer();
    void Bind() const;
    void Unbind() const;

private:
    GLuint m_RendererID;
    GLuint m_Count;
};