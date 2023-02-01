#pragma once

#include <map>

#include <ft2build.h>
#include <freetype/freetype.h>

#include "../../opengl/Shader.h"
#include "../../utils/TextureAtlas.h"
#include "../../primitives/Quad.h"

struct Character
{
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Offset to advance to next glyph
    unsigned int offset;
};

class Font
{
    friend class Text;
public:
    Font(const std::string& path, int size);

private:
    std::map<unsigned int, Character> m_Symbols;
    std::string m_Path;
    int m_Size;
    TextureAtlas m_TextureAtlas;
    bool LoadFont(std::string path , int size);
};