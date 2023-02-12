#pragma once

#include "Renderer.h"

class TextureArray
{
public:
    TextureArray(GLuint format);
    ~TextureArray();

    void Bind() const;
    void Unbind() const;

    void SetInternalFormat(GLuint format);

    void Resize(glm::vec3 size);
    void SetData(glm::vec2 pos, unsigned int layer, glm::vec2 size, unsigned char* data);
    glm::vec3 getSize() {return m_Size;}

private:
    GLuint m_RendererID;
    GLuint m_InternalFormat;
    glm::vec3 m_Size;
};