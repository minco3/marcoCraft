#pragma once

#include "Renderer.h"
#include "Texture2D.h"

#include <memory>
#include <map>

struct FrameBufferAttachment
{
    FrameBufferAttachment(GLuint type, glm::uvec2 size, GLuint format)
        : attachmentType(type), texture(std::make_shared<Texture2D>(size, format)) {}

    GLuint attachmentType;
    std::shared_ptr<Texture2D> texture;
};

class FrameBuffer
{
public:
    FrameBuffer();
    FrameBuffer(const glm::uvec2& size);
    ~FrameBuffer();

    void AddAttachment(GLuint attachmentType, GLuint format);

    void Bind();
    void BindBuffers();
    void BindTexture(GLuint TextureSlot, GLuint Attachment);
    void Unbind();

    void CheckFrameBuffer();
    
private:
    GLuint m_RendererID;
    std::map<GLuint, std::shared_ptr<Texture2D>> m_Attachments;
    glm::uvec2 m_Size;
};