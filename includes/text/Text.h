#pragma once

#include <algorithm>
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TextAtlas.h"
#include "../opengl/VertexArray.h"

class Text
{
public:
    Text(const TextAtlas& textAtlas, std::string font, std::string string = std::string());
    
    void SetString(std::string string);
    std::string GetString();

    void RenderText();

private:
    glm::mat4 m_Projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    TextAtlas m_TextAtlas;
    std::string m_Font;
    std::string m_String;
    GLuint m_ShaderID;
    glm::vec2 m_Position;
    glm::vec3 m_Color;
    VertexArray va;
    float m_Scale;

};