#include "World.h"

#include "external/SimplexNoise/SimplexNoise.h"

void World::GenerateChunk(glm::ivec2 pos)
{
    m_Chunks.emplace_back(&m_Worldgen, pos);
}