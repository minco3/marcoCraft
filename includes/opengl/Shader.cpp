#include "Shader.h"

#include <filesystem>

#include <map>

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    m_RendererID = CompileShader();
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glProgramUniform1i(m_RendererID, GetUniformLocation(name), value));
}

void Shader::SetUniform2f(const std::string& name, const glm::vec2& value)
{
    GLCall(glProgramUniform2f(m_RendererID, GetUniformLocation(name), value.x, value.y));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
{
    GLCall(glProgramUniform3f(m_RendererID, GetUniformLocation(name), value.x, value.y, value.z));
}

void Shader::SetUniform4fv(const std::string& name, const glm::mat4& data)
{
    // GLCall(glProgramUniform4fv(GetUniformLocation(name), 1, GL_FALSE, &data[0][0]));
}

void Shader::SetUniformMat4fv(const std::string& name, const glm::mat4& mat)
{
    GLCall(glProgramUniformMatrix4fv(m_RendererID, GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]));
}

GLuint Shader::GetUniformLocation(const std::string& name)
{
    GLCall(GLint location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform '" << name << "' doesnt't exist!" << std::endl;

    return location;
}

GLuint Shader::CompileShader()
{
    std::ifstream stream(m_FilePath);

    if (!stream.is_open())
    {
        std::cout << "ERROR: can not find shader '" << m_FilePath << "'" << std::endl;
        return 0;
    }
    
    enum ShaderType
    {
        NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2
    };

    struct ShaderSpec
    {
        std::string shaderName;
        GLenum ShaderEnum;
    };

    struct ShaderSource
    {
        GLenum ShaderID;
        std::stringstream ShaderSrc;
    };

    static const std::map<ShaderType, ShaderSpec> ShaderTypes = {
        {VERTEX, {"Vertex", GL_VERTEX_SHADER}}, {GEOMETRY, {"Geometry", GL_GEOMETRY_SHADER}}, {FRAGMENT, {"Fragment", GL_FRAGMENT_SHADER}}
    };

    std::map<ShaderType, ShaderSource> Shaders;


    std::string line;
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
                GLCall(GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER));
                Shaders.insert(std::make_pair<ShaderType, ShaderSource>(VERTEX, {VertexShaderID, std::stringstream()}));
            }
            else if (line.find("geometry") != std::string::npos)
            {
                type = ShaderType::GEOMETRY;
                GLCall(GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER));
                Shaders.insert(std::make_pair<ShaderType, ShaderSource>(GEOMETRY, {GeometryShaderID, std::stringstream()}));

            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
                GLCall(GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER));
                Shaders.insert(std::make_pair<ShaderType, ShaderSource>(FRAGMENT, {FragmentShaderID, std::stringstream()}));

            }
        }
        else
        {
            Shaders.at(type).ShaderSrc << line << '\n';
        }
    }

    if (!Shaders.count(VERTEX) || !Shaders.count(FRAGMENT))
    {
        std::cout << "ERROR: shader '" << m_FilePath << "' needs both a vertex and a fragment shader" << std::endl;
    }

    for (const auto& it : Shaders)
    {
        ShaderType type = it.first;
        const ShaderSource& ShaderSrc = it.second;

        GLuint ShaderID = ShaderSrc.ShaderID;

        int InfoLogLength;
        GLint result = GL_FALSE;

        std::cout << "Compiling " << ShaderTypes.at(type).shaderName << " Shader" << std::endl;
        std::string code = ShaderSrc.ShaderSrc.str();
        char const * ShaderSourcePointer = code.c_str();
        GLCall(glShaderSource(ShaderID, 1, &ShaderSourcePointer, NULL));
        GLCall(glCompileShader(ShaderID));
        
        GLCall(glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &result));
        GLCall(glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength));

        if (InfoLogLength > 0)
        {
            std::vector<char> ErrorMessage(InfoLogLength+1);
            GLCall(glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ErrorMessage[0]));
            std::cout << &ErrorMessage[0] << '\n';
        }
    }

    std::cout << "Linking program\n";
    GLCall(GLuint ProgramID = glCreateProgram());
    for (const auto& it : Shaders)
    {
        GLCall(glAttachShader(ProgramID, it.second.ShaderID));
    }
    GLCall(glLinkProgram(ProgramID));

    for (const auto& it : Shaders)
    {
        GLCall(glDeleteShader(it.second.ShaderID));
    }

    return ProgramID;

}


std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
    m_Shaders.insert({name, std::make_shared<Shader>(filepath)});
    return Get(name);
}

std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath)
{
    std::filesystem::path path(filepath);
    return Load(path.stem().string(), filepath);
}


std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
{
    assert(Exists(name));
    return m_Shaders.at(name);
}

bool ShaderLibrary::Exists(const std::string& name) const
{
    return m_Shaders.count(name);
}
