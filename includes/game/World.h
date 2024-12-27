#pragma once

#include "external/SimplexNoise/SimplexNoise.h"
#include <glm/glm.hpp>
#include <map>
#include <vector>

#include "Chunk.h"

#include "includes/utils/Timer.h"
#include "includes/opengl/VertexArray.h"

class World
{
public:
    void GenerateChunk(glm::ivec2 pos);
    void InvisibleCulling(uint8_t* buffer);

    const std::vector<Chunk>& Chunks() { return m_Chunks; }

    uint32_t numVertices() const { return m_NumVertices; }

    void BindVA() { m_VertexArray.Bind(); }

private:
    VertexArray m_VertexArray;
    uint32_t m_NumVertices;
    std::vector<Chunk> m_Chunks;
    SimplexNoise m_Worldgen;
};