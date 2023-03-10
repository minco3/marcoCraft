#pragma once

#include "Renderer.h"

class Texture2D
{
friend class FrameBuffer;
public:
    Texture2D();
    Texture2D(const glm::uvec2 size, GLuint internalFormat = GL_RGBA);
    ~Texture2D();

    void Bind() const;
    void Unbind() const;

    void SetInternalFormat(GLuint format);

    void Resize(glm::uvec2 size);
    void SetData(glm::vec2 pos, glm::uvec2 size, unsigned char* data);
    void SetData(glm::vec2 pos, glm::uvec2 size, float* data);
    glm::uvec2 getSize() {return m_Size;}

private:
    GLuint m_RendererID;
    GLuint m_InternalFormat;
    GLuint m_DataFormat;
    GLuint m_DataType;
    glm::uvec2 m_Size;
};