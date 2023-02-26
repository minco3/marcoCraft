#include "Texture2D.h"

Texture2D::Texture2D() 
{
    GLCall(glGenTextures(1, &m_RendererID));
    Bind();
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

Texture2D::Texture2D(const glm::uvec2 size, GLuint internalFormat)
    : Texture2D() // delegating constructor
{
    SetInternalFormat(internalFormat);
    Resize(size);
}

Texture2D::~Texture2D()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture2D::Bind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture2D::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::SetInternalFormat(GLuint format)
{
    m_InternalFormat = format;
    switch (format)
    {
    case GL_RED:
        m_DataFormat = GL_RED;
        m_DataType = GL_UNSIGNED_BYTE;
    case GL_RGB:
        m_DataFormat = GL_RGB;
        m_DataType = GL_UNSIGNED_BYTE;
        break;
    case GL_RGBA:
        m_DataFormat = GL_RGBA;
        m_DataType = GL_UNSIGNED_BYTE;
        break;
    case GL_DEPTH_COMPONENT:
        m_DataFormat = GL_DEPTH_COMPONENT;
        m_DataType = GL_UNSIGNED_BYTE;
        break;
    case GL_RGB16F:
    case GL_RGB32F:
        m_DataFormat = GL_RGB;
        m_DataType = GL_FLOAT;
        break;
    case GL_RGBA16F:
        m_DataFormat = GL_RGBA;
        m_DataType = GL_FLOAT;
        break;
    }
}

void Texture2D::Resize(glm::uvec2 size)
{
    m_Size = size;
    Bind();
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Size.x, m_Size.y, 0, m_DataFormat, m_DataType, 0));
}

void Texture2D::SetData(glm::vec2 pos, glm::uvec2 size, unsigned char* data)
{
    Bind();
    GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, size.x, size.y, m_InternalFormat, GL_UNSIGNED_BYTE, data));
}

void Texture2D::SetData(glm::vec2 pos, glm::uvec2 size, float* data)
{
    Bind();
    GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, size.x, size.y, m_InternalFormat, GL_FLOAT, data));
}

