#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const GLuint * data, unsigned int count) 
    : m_Count(count)
{
    GLCall(glGenBuffers(1, &m_BufferID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_BufferID));
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID));
}

void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}