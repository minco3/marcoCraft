#include "Shader.h"

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
    GLCall(GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER));
    GLCall(GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER));

    std::ifstream stream(m_FilePath);

    if (!stream.is_open())
    {
        std::cout << "ERROR: can not find shader '" << m_FilePath << "'" << std::endl;
        return 0;
    }
    
    enum ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)type] << line << '\n';
        }
    }

    for (int i=0; i<2; i++)
    {
        GLuint ShaderID = i == 0 ? VertexShaderID : FragmentShaderID;

        std::cout << "Compiling " << (i == 0 ? "Vertex" : "Fragment") << " Shader" << std::endl;
        std::string code = ss[i].str();
        char const * VertexSourcePointer = code.c_str();
        GLCall(glShaderSource(ShaderID, 1, &VertexSourcePointer, NULL));
        GLCall(glCompileShader(ShaderID));

    }

    std::cout << "Linking program\n";
    GLCall(GLuint ProgramID = glCreateProgram());
    GLCall(glAttachShader(ProgramID, VertexShaderID));
    GLCall(glAttachShader(ProgramID, FragmentShaderID));
    GLCall(glLinkProgram(ProgramID));

    GLCall(glDetachShader(ProgramID, VertexShaderID));
    GLCall(glDetachShader(ProgramID, FragmentShaderID));

    GLCall(glDeleteShader(VertexShaderID));
    GLCall(glDeleteShader(FragmentShaderID));

    return ProgramID;

}
