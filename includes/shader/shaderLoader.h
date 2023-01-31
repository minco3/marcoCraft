#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <GL/glew.h>
#include <gl/GL.h>

#define GL_LOG_ERROR(ProgramID, InfoLogLength) { \
    std::vector<char> ErrorMessage(InfoLogLength+1); \
    glGetShaderInfoLog(ProgramID, InfoLogLength, NULL, &ErrorMessage[0]); \
    std::cout << &ErrorMessage[0] << '\n'; \
}

GLuint loadShaders(const char * shader_file_path);
