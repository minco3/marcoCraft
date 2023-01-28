#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <gl/GL.h>
#include <GL/glew.h>

#define GL_LOG_ERROR(ProgramID, InfoLogLength) { \
    std::vector<char> ErrorMessage(InfoLogLength+1); \
    glGetShaderInfoLog(ProgramID, InfoLogLength, NULL, &ErrorMessage[0]); \
    std::cout << &ErrorMessage[0] << '\n'; \
}

GLuint loadShaders(const char * shader_file_path) {
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    std::ifstream stream(shader_file_path);
    
    enum ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            ss[(int)type] << line << '\n';
        }
    }

    GLint result = GL_FALSE;
    int InfoLogLength;


    for (int i=0; i<2; i++) {
        GLuint ShaderID = i == 0 ? VertexShaderID : FragmentShaderID;

        std::cout << "Compiling " << (i == 0 ? "Vertex" : "Fragment") << " Shader" << std::endl;
        std::string code = ss[i].str();
        char const * VertexSourcePointer = code.c_str();
        glShaderSource(ShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(ShaderID);

        glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &result);
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            GL_LOG_ERROR(ShaderID, InfoLogLength)
        }
    }

    std::cout << "Linking program\n";
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        GL_LOG_ERROR(ProgramID, InfoLogLength)
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;

}
