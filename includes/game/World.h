#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "external/SimplexNoise/SimplexNoise.h"

#include "Chunk.h"

#include "includes/utils/Timer.h"

class World
{
    public:
    void GenerateChunk(glm::ivec2 pos);

    const std::vector<Chunk>& Chunks() { return m_Chunks; }

    private:
    std::vector<Chunk> m_Chunks;
    SimplexNoise m_Worldgen;
};