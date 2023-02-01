#include "Texture2D.h"

Texture2D::Texture2D(GLint internalFormat, const GLuint * data, GLuint width, GLuint height) 
    : m_Width(width), m_Height(height), m_InternalFormat(internalFormat)
{
    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, internalFormat, GL_UNSIGNED_BYTE, 0));
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

void Texture2D::Resize(glm::vec2 size) {
    Bind();
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_InternalFormat, GL_UNSIGNED_BYTE, 0));
}