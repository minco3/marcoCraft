#pragma once

#include <map>

#include <ft2build.h>
#include <freetype/freetype.h>

#include "includes/opengl/Shader.h"
#include "includes/utils/TextureAtlas.h"
#include "includes/primitives/TexturedQuad.h"

struct Character
{
    glm::vec2   AtlasCoord;
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Offset to advance to next glyph
};

class Font
{
public:
    Font(const std::string& path, int size);

    void Bind();

    glm::ivec2 getSize() { return m_TextureAtlas.getSize(); }

    const Character& getCharacter(char c) const { return m_Characters.count(c) ? m_Characters.at(c) : m_Characters.at('0'); }

private:
    std::map<char, Character> m_Characters;
    Texture2D m_TextureAtlas;
    std::string m_Path;
    int m_Size;
    bool LoadFont(std::string path , int size);
};