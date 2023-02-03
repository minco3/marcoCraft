#pragma once

#include <algorithm>
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Font.h"
#include "../../opengl/VertexArray.h"
#include "../../primitives/TexturedQuad.h"

class Text
{
public:
    Text(const Font& font, std::string string = std::string());
    
    void SetString(std::string string);
    std::string GetString();

    void setPosition(glm::vec2);

    void RenderText();

private:
    std::string m_String;
    Font m_Font;
    Shader m_Shader;
    glm::mat4 m_Projection;
    glm::vec2 m_Position;
    glm::vec3 m_Color;
    Quad m_Quad;
    float m_Scale;

};