#pragma once

#include <map>

#include <ft2build.h>
#include <freetype/freetype.h>

#include "../../opengl/Shader.h"
#include "../../utils/TextureAtlas.h"
#include "../../primitives/TexturedQuad.h"

struct Character
{
    glm::ivec2   AtlasCoord;
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    FT_Pos Advance;    // Offset to advance to next glyph
};

class Font
{
    friend class Text;
public:
    Font(const std::string& path, int size);

    void Bind();

private:
    std::map<char, Character> m_Characters;
    Texture2D m_TextureAtlas;
    std::string m_Path;
    int m_Size;
    bool LoadFont(std::string path , int size);
};