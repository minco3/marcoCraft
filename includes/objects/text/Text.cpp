#include "Text.h"

Text::Text(const Font& font, std::string String)
    : m_Projection(glm::ortho(0.0f, 800.0f, 0.0f, 600.0f)), m_Font(font), m_Color(1.0f, 1.0f, 1.0f),
    m_Scale(1), m_Shader("../includes/text/TextShader.glsl")
{}
    
void Text::SetString(std::string string)
{
    if (m_String != string) {
        m_String = string;
    }
}

std::string Text::GetString()
{
    return m_String;
}

void Text::RenderText()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_Shader.SetUniform3f("textColor", m_Color);
    m_Shader.SetUniform4fv("projection", m_Projection);
    glActiveTexture(GL_TEXTURE0);
    m_Shader.SetUniform1i("text", 0);


    float x = m_Position.x;

    for (char c : m_String) {
        Character ch = m_Font.m_Symbols[c];

        float xpos = x + ch.Bearing.x;
        float ypos = m_Position.y - (ch.Size.y - ch.Bearing.y);

        float w = ch.Size.x * m_Scale;
        float h = ch.Size.y * m_Scale;

        m_Quad.UpdateVertices(glm::vec2(xpos, ypos), glm::vec2(w, h));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        x += (ch.Advance >> 6) * m_Scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBlendFunc(GL_ONE, GL_ZERO);
}