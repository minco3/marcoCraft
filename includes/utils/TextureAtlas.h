#pragma once

#include "../opengl/Texture2D.h"

class TextureAtlas
{
    friend class Text;

public:
    TextureAtlas(GLint internalFormat);

private:
    Texture2D m_AtlasTexture;
};