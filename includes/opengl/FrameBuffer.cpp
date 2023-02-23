#include "FrameBuffer.h"

#include "includes/application/Application.h"

#include <vector>

FrameBuffer::FrameBuffer()
{
    m_Size = Application::Get().WindowSize();
    GLCall(glGenFramebuffers(1, &m_RendererID));
}

FrameBuffer::FrameBuffer(const glm::uvec2& size)
    : m_Size(size)
{
    GLCall(glGenFramebuffers(1, &m_RendererID));
}

FrameBuffer::~FrameBuffer()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::AddAttachment(GLuint attachmentType, GLuint format)
{
    Bind();
    m_Attachments.insert({attachmentType, std::make_shared<Texture2D>(m_Size, format)});
    GLCall(glFramebufferTexture2D(
        GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, m_Attachments.at(attachmentType)->m_RendererID, 0));
}

void FrameBuffer::Bind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::BindBuffers()
{
    unsigned int bufferCount = 0;
    std::vector<GLenum> buffers;
    Bind();
    for (const auto& Attachment : m_Attachments)
    {
        if (Attachment.first == GL_DEPTH_ATTACHMENT) continue;
        buffers.push_back(Attachment.first);
        bufferCount++;
    }
    GLCall(glDrawBuffers(bufferCount, buffers.data()));
}

void FrameBuffer::BindTexture(GLuint TextureSlot, GLuint Attachment)
{
    GLCall(glActiveTexture(TextureSlot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_Attachments.at(Attachment)->m_RendererID));
}

void FrameBuffer::Unbind()
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::CheckFrameBuffer()
{
    Bind();
    GLCall(GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer error " << gluErrorString(result) << std::endl;      
    }
}