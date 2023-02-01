#pragma once

#include "renderer.h"

class Texture2D
{
public:
    Texture2D(GLint internalFormat, const GLuint * data = nullptr, GLuint width = 0, GLuint height = 0);
    ~Texture2D();

    void Bind() const;
    void Unbind() const;

    void Resize(glm::vec2 size);

private:
    GLuint m_RendererID;
    GLuint m_Width;
    GLuint m_Height;
    GLuint m_InternalFormat;
};