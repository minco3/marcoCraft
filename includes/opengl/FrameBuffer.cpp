#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(const glm::ivec2& size)
    : m_Size(size), Texture(size, GL_RGBA), NormalTexture(size, GL_RGB), DepthTexture(size, GL_DEPTH_COMPONENT)
{
    GLCall(glGenFramebuffers(1, &m_RendererID));
    Bind();
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.m_RendererID, 0));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalTexture.m_RendererID, 0));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture.m_RendererID, 0));
}

FrameBuffer::~FrameBuffer()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::Bind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::Unbind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}