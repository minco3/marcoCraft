#include "Text.h"

Text::Text(const std::shared_ptr<Font>& font, std::string String)
    :m_Font(font), m_Color(1.0f, 1.0f, 1.0f),
    m_Scale(1), m_Shader("/Users/marcomiralles/src/marcoCraft/res/shaders/TextShader.glsl")
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

void Text::setPosition(glm::vec2 position)
{
    m_Position = position;
}

void Text::SetScreenSize(int width, int height)
{
    m_Projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

void Text::RenderText()
{
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE1);
    m_Quad.Bind();
    m_Quad.BindVB();
    m_Shader.Bind();
    m_Shader.SetUniform3f("textColor", m_Color);
    m_Shader.SetUniformMat4fv("projection", m_Projection);
    m_Font->Bind();
    m_Shader.SetUniform1i("text", 1);

    float x = m_Position.x;
    const glm::vec2& atlasSize = m_Font->getSize();

    const int lineHeight = 50;

    unsigned linecount = 0;

    for (char c : m_String) {
        
        if (c == '\n')
        {
            linecount++;
            x = m_Position.x;
            continue;
        }

        const Character& ch = m_Font->getCharacter(c);

        float xpos = x + ch.Bearing.x;
        float ypos = m_Position.y - (ch.Size.y - ch.Bearing.y) - (linecount * lineHeight);

        float w = ch.Size.x * m_Scale;
        float h = ch.Size.y * m_Scale;

        glm::vec2 texturePos(ch.AtlasCoord.x / atlasSize.x, 0);
        glm::vec2 textureSize(w / atlasSize.x, h / atlasSize.y);


        m_Quad.UpdateBuffer(glm::vec2(xpos, ypos), glm::vec2(w, h), texturePos, textureSize);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        x += (ch.Advance >> 6) * m_Scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBlendFunc(GL_ONE, GL_ZERO);
    glEnable(GL_DEPTH_TEST);
}
