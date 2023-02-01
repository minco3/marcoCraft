#pragma once

#include <assert.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/glm.hpp>

class VertexArray;
class Shader;

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}
static bool GLLogCall(const char* function, const char * file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}
#define GLCall(x) GLClearError();\
    x;\
    assert(GLLogCall(#x, __FILE__, __LINE__))


class Renderer
{
public:
    void Draw(const VertexArray& va, const Shader& shader) const;

private:

};