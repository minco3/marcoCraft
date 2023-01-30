#include <algorithm>
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <memory>

#include "TextAtlas.h"
#include "../shader/shaderLoader.h"

class Text
{
    public:
    Text(std::shared_ptr<TextAtlas> TextAtlas, std::string font, std::string string = std::string());
    
    void SetString(std::string string);
    std::string GetString();

    void RenderText();

    private:
    glm::mat4 m_projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    std::shared_ptr<TextAtlas> m_TextAtlas;
    std::string m_Font;
    std::string m_String;
    GLuint m_ShaderID;
    glm::vec3 m_Color;
    float m_Scale;

};