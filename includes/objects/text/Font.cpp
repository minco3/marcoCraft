#include "font.h"

Font::Font(const std::string& path, int size)
    : m_TextureAtlas(GL_RED)
{

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

    GLuint texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        w,
        h,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        0
    );

    unsigned int x = 0;

    for (unsigned char i=32; i<128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            x,
            0,
            g->bitmap.width,
            g->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            g->bitmap.buffer);

        x += g->bitmap.width;


        Character character = {
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left, g->bitmap_top),
            g->advance.x,
        };

        m_Symbols.insert({i, character});
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}