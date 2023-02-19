#include "TextureArray.h"

TextureArray::TextureArray(GLuint format) 
    : m_InternalFormat(format)
{
    GLCall(glGenTextures(1, &m_RendererID));
    Bind();
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

TextureArray::~TextureArray()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

void TextureArray::Bind() const
{
    glActiveTexture(GL_TEXTURE1);
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_RendererID));
}

void TextureArray::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}

void TextureArray::SetInternalFormat(GLuint format)
{
    m_InternalFormat = format;
}

void TextureArray::Resize(glm::ivec3 size)
{
    m_Size = size;
    Bind();
    GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, m_InternalFormat, m_Size.x, m_Size.y, m_Size.z, 0, m_InternalFormat, GL_UNSIGNED_BYTE, 0));
}

void TextureArray::SetData(glm::vec2 pos, unsigned int layer, glm::vec2 size, unsigned char* data)
{
    Bind();
    GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, pos.x, pos.y, layer, size.x, size.y, 1, m_InternalFormat, GL_UNSIGNED_BYTE, data));
}