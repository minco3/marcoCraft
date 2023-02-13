#pragma once

#include <memory>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;

private:
    GLuint m_RendererID;
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
};