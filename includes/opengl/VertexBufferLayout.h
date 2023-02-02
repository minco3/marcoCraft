#pragma once

#include <vector>

#include "Renderer.h"

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned int normalized;

    static unsigned int SizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:          return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
        }
        assert(false);
        return 0;
    }
};

class VertexBufferLayout
{
public:
    VertexBufferLayout()
        : m_Stride(0) {}

    void Push(GLuint type, unsigned int count)
    {
        m_Elements.push_back({type, count, GL_FALSE});
        
        
        m_Stride += count * VertexBufferElement::SizeOfType(type);
    }

    inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
    inline unsigned int getStride() const { return m_Stride; }

private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;
};
