#pragma once

#include <array>
#include <glm/glm.hpp>
#include "external/SimplexNoise/SimplexNoise.h"

struct Chunk
{
public:
    Chunk(SimplexNoise* gen, glm::ivec2 pos);
    glm::ivec2 pos;
    std::array<std::array<std::array<int, 16>, 128>, 16> data;
};
