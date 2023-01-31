#pragma once

#include <map>
#include <ft2build.h>
#include <freetype/freetype.h>

#include "../opengl/Texture2D.h"

struct Character
{
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Offset to advance to next glyph
    unsigned int offset;
};

class TextAtlas
{
    friend class Text;

public:
    TextAtlas(std::string path, int size);
    loadFont(std::string path, int size);

private:
    Texture2D m_AtlasTexture;
    std::map<char, Character> m_Characters;
    std::string m_Path;
    int m_Size;
};