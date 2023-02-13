#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "Renderer.h"

class Shader
{
public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int value);
    void SetUniform2f(const std::string& name, const glm::vec2& value);
    void SetUniform3f(const std::string& name, const glm::vec3& value);
    void SetUniform4fv(const std::string& name, const glm::mat4& value);
    void SetUniformMat4fv(const std::string& name, const glm::mat4& mat);

private:
    GLuint GetUniformLocation(const std::string& name);
    GLuint CompileShader();
    GLuint m_RendererID;
    std::string m_FilePath;
};

class ShaderLibrary
{
public:
    std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);
    std::shared_ptr<Shader> Load(const std::string& filepath);

    std::shared_ptr<Shader> Get(const std::string& name);

    bool Exists(const std::string& name) const;

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
};