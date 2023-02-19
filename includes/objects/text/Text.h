#pragma once

#include <algorithm>
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Font.h"
#include "includes/opengl/VertexArray.h"
#include "includes/primitives/TexturedQuad.h"

class Text
{
public:
    Text(const std::shared_ptr<Font>& font, std::string string = std::string());
    
    void SetString(std::string string);
    std::string GetString();

    void setPosition(glm::vec2);

    void SetScreenSize(int width, int height);

    void RenderText();

private:
    std::string m_String;
    std::shared_ptr<Font> m_Font;
    Shader m_Shader;
    glm::mat4 m_Projection;
    glm::vec2 m_Position;
    glm::vec3 m_Color;
    Quad m_Quad;
    float m_Scale;

};