#include "Font.h"

Font::Font(const std::string& path, int size)
{
    m_TextureAtlas.SetInternalFormat(GL_RED);
    GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    LoadFont(path, size);
}

void Font::Bind()
{
    m_TextureAtlas.Bind();
}

bool Font::LoadFont(std::string path , int size)
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        std::cout << "Could not init FreeType" << std::endl;
        return false;
    }

    FT_Face face;

    if (FT_New_Face(ft, path.c_str(), 0, &face)) {
        std::cout << "Failed to load font" << std::endl;  
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    unsigned int w=0, h=0;

    FT_GlyphSlot g = face->glyph;

    for (unsigned char c=32; c<128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "Failed to load Glyph " << char(c) << std::endl;
            continue;
        }

        w += g->bitmap.width;
        h = std::max(h, g->bitmap.rows);
        
    }

    m_TextureAtlas.Resize(glm::ivec2(w, h));

    unsigned int x = 0;

    for (unsigned char i=32; i<128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        m_TextureAtlas.SetData(glm::vec2(x, 0), glm::vec2(g->bitmap.width, g->bitmap.rows), g->bitmap.buffer);

        Character character = {
            glm::ivec2(x, 0),
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left, g->bitmap_top),
            g->advance.x,
        };
        m_Characters.insert({i, character});


        x += g->bitmap.width;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}