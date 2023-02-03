#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"


class Shader
{
public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int value);
    void SetUniform2i(const std::string& name, const glm::vec2& value);
    void SetUniform3f(const std::string& name, const glm::vec3& value);
    void SetUniform4fv(const std::string& name, const glm::mat4& value);
    void SetUniformMat4fv(const std::string& name, const glm::mat4& mat);

private:
    GLuint GetUniformLocation(const std::string& name);
    GLuint CompileShader();
    GLuint m_RendererID;
    std::string m_FilePath;
};