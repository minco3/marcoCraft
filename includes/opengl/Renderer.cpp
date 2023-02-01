#include "renderer.h"

#include "Shader.h"
#include "VertexArray.h"

#include <iostream>

void Renderer::Draw(const VertexArray& va, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    

}


