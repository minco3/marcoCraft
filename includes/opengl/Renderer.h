#pragma once

#include <GL/glew.h>
#include <GL/glu.h>
#include <assert.h>
#include <glm/glm.hpp>
#include <iostream>
#include <print>

class VertexArray;
class Shader;

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}
static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::println(
            "[OpenGL Error] ({}): {} {}:{}",
            reinterpret_cast<const char*>(gluErrorString(error)), function,
            file, line);
        return false;
    }
    return true;
}
#define GLCall(x)                                                              \
    GLClearError();                                                            \
    x;                                                                         \
    assert(GLLogCall(#x, __FILE__, __LINE__))

class Renderer
{
public:
    void Draw(const VertexArray& va, const Shader& shader) const;

private:
};