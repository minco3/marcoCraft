#pragma once

#include "renderer.h"

class Texture2D
{
public:
    Texture2D();
    ~Texture2D();

    void Bind() const;
    void Unbind() const;

    void SetInternalFormat(GLuint format);

    void Resize(glm::vec2 size);
    void SetData(glm::vec2 pos, glm::vec2 size, unsigned char* data);
    glm::vec2 getSize() {return m_Size;}

private:
    GLuint m_RendererID;
    GLuint m_InternalFormat;
    glm::vec2 m_Size;
};