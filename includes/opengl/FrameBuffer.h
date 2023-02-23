#pragma once

#include "Renderer.h"
#include "Texture2D.h"

class FrameBuffer
{
public:
    FrameBuffer(const glm::ivec2& size);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    Texture2D Texture;
    Texture2D NormalTexture;
    Texture2D DepthTexture;
    Texture2D PositionTexture;
    
private:
    GLuint m_RendererID;
    glm::ivec2 m_Size;
};