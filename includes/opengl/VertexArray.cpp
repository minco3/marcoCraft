#include "VertexArray.h"

VertexArray::VertexArray()
{
    GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}

void VertexArray::AddBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, const VertexBufferLayout& layout) {
   
    Bind();
    vertexBuffer->Bind();
    const auto& elements = layout.GetElements();

    uint64_t offset = 0;

    for (int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.count, element.type,
            element.normalized, layout.getStride(), (const void*)offset));
        offset += element.count * VertexBufferElement::SizeOfType(element.type);
    }

    m_VertexBuffers.push_back(vertexBuffer);
}
