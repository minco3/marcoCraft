#include "Text.h"

#include "../shader/shaderLoader.h"

Text::Text(const TextAtlas& textAtlas, std::string Font, std::string String)
    : m_TextAtlas(textAtlas), m_Color(1.0f, 1.0f, 1.0f), m_Scale(1)
{
    m_ShaderID = loadShaders("../includes/text/TextShader.glsl");

}
    
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
    glUseProgram(m_ShaderID);
    va.Bind();
    glUniform3f(glGetUniformLocation(m_ShaderID, "textColor"), m_Color.r, m_Color.g, m_Color.b);
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, "projection"), 1, GL_FALSE, &m_Projection[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(m_ShaderID, "text"), 0);

    float x = m_Position.x;

    for (char c : m_String) {
        Character ch = m_TextAtlas.m_Characters[c];

        float xpos = x + ch.Bearing.x;
        float ypos = m_Position.y - (ch.Size.y - ch.Bearing.y);

        float w = ch.Size.x * m_Scale;
        float h = ch.Size.y * m_Scale;

        float vertices[4][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f}, // 0            
            { xpos,     ypos,       0.0f, 0.0f}, // 1
            { xpos + w, ypos,       1.0f, 0.0f}, // 2
            { xpos + w, ypos + h,   1.0f, 1.0f}  // 3
        };

        // float vertices[4][4] = {
        //     { xpos,     ypos + h,   ch.offset, (float)ch.Size.y / ch.atlasHeight}, // 0            
        //     { xpos,     ypos,       ch.offset, 0.0f}, // 1
        //     { xpos + w, ypos,       ch.offset + (float)ch.Size.x, 0.0f}, // 2
        //     { xpos + w, ypos + h,   (float)ch.Size.y / ch.atlasHeight, (float)ch.Size.y / ch.atlasHeight }  // 3
        // };

        // for (int i=0; i<4; i++) {
        //     for (int j=0; j<4; j++) {
        //         std::cout << vertices[i][j] << ' ';
        //     }
        //     std::cout << '\n';
        // }


        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 4 * 4, vertices);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        x += (ch.Advance >> 6) * m_Scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBlendFunc(GL_ONE, GL_ZERO);
}