#include "Texture2D.h"

Texture2D::Texture2D(const GLuint * data, GLuint width, GLuint height, GLint internalFormat) 
    : m_Width(width), m_Height(height)
{
    GLCall(glGenTextures(1, &m_TextureID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, internalFormat, GL_UNSIGNED_BYTE, 0));
}

Texture2D::~Texture2D()
{
    GLCall(glDeleteTextures(1, &m_TextureID));
}

void Texture2D::Bind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
}

void Texture2D::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}