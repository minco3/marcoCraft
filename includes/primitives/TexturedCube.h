#pragma once

#include "../opengl/VertexArray.h"
#include "../opengl/VertexBuffer.h"
#include "../opengl/IndexBuffer.h"

struct Model {
    int front;
    int back;
    int left;
    int right;
    int top;
    int bottom;
    
};

class Cube
{
public:
    Cube();

    unsigned int IndexCount() const { return 36; }
    
    void Bind() const;
    void BindVB() const;

    void UpdateBuffer(glm::vec3 pos, glm::vec3 size, glm::vec2 texturePos, glm::vec2 textureSize, const Model& m);

private:

    VertexArray m_VertexArray;
    VertexBuffer m_VertexBuffer;

};

