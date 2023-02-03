#include "Texture2D.h"

Texture2D::Texture2D() 
{
    GLCall(glGenTextures(1, &m_RendererID));
    Bind();
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

Texture2D::~Texture2D()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture2D::Bind() const
{
    glActiveTexture(GL_TEXTURE0);
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture2D::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::SetInternalFormat(GLuint format)
{
    m_InternalFormat = format;
}

void Texture2D::Resize(glm::vec2 size)
{
    m_Size = size;
    Bind();
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Size.x, m_Size.y, 0, m_InternalFormat, GL_UNSIGNED_BYTE, 0));
}

void Texture2D::SetData(glm::vec2 pos, glm::vec2 size, unsigned char* data)
{
    Bind();
    GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, size.x, size.y, m_InternalFormat, GL_UNSIGNED_BYTE, data));
}