#pragma once

#include "renderer.h"

class Texture2D
{
    public:
    Texture2D(const GLuint * data = nullptr, GLuint width = 0, GLuint height = 0, GLint internalFormat = 0);
    ~Texture2D();
    void Bind() const;
    void Unbind() const;

    private:
    GLuint m_TextureID;
    GLuint m_Width;
    GLuint m_Height;
};