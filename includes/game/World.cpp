#include "World.h"
#include "../primitives/TexturedCube.h"

#include "external/SimplexNoise/SimplexNoise.h"

void World::GenerateChunk(glm::ivec2 pos)
{
    m_Chunks.emplace_back(&m_Worldgen, pos);
}

void World::InvisibleCulling(uint8_t* buffer)
{
    SimplexNoise worldgen;

    struct Block
    {
        std::array<bool, 6> facesVisible;
        glm::ivec3 position;
        int model;
    };

    for (int x = -2; x <= 2; x++)
    {
        for (int z = -2; z <= 2; z++)
        {
            GenerateChunk({x, z});
        }
    }

    std::vector<std::shared_ptr<Cube>> solidBlocks, grassBlocks,
        transparentBlocks;

    std::vector<Block> visibleBlocks[2];

    Timer invisibleCullingTimer("Invisible face culling");

    for (const Chunk& chunk : Chunks())
    {
        for (int x = 0; x < chunk.data.size(); x++)
        {
            for (int y = 0; y < chunk.data[x].size(); y++)
            {
                for (int z = 0; z < chunk.data[x][y].size(); z++)
                {
                    bool isVisible = false;
                    std::array<bool, 6> faces{false};

                    if (!chunk.data[x][y][z])
                        continue;

                    if (x == 0 || !chunk.data[x - 1][y][z])
                    {
                        isVisible = true;
                        faces[0] = true;
                    }
                    if (y == 0 || !chunk.data[x][y - 1][z])
                    {
                        isVisible = true;
                        faces[1] = true;
                    }
                    if (z == 0 || !chunk.data[x][y][z - 1])
                    {
                        isVisible = true;
                        faces[2] = true;
                    }
                    if (x == chunk.data.size() - 1 || !chunk.data[x + 1][y][z])
                    {
                        isVisible = true;
                        faces[3] = true;
                    }
                    if (y == chunk.data[0].size() - 1 ||
                        !chunk.data[x][y + 1][z])
                    {
                        isVisible = true;
                        faces[4] = true;
                    }
                    if (z == chunk.data[0][0].size() - 1 ||
                        !chunk.data[x][y][z + 1])
                    {
                        isVisible = true;
                        faces[5] = true;
                    }

                    if (isVisible)
                    {
                        // grass
                        if (chunk.data[x][y][z] == 3)
                        {
                            visibleBlocks[1].push_back(
                                {faces,
                                 {chunk.pos.x * 16 + x, y,
                                  chunk.pos.y * 16 + z},
                                 chunk.data[x][y][z]});
                        }
                        else
                        {
                            visibleBlocks[0].push_back(
                                {faces,
                                 {chunk.pos.x * 16 + x, y,
                                  chunk.pos.y * 16 + z},
                                 chunk.data[x][y][z]});
                        }
                    }
                }
            }
        }
    }

    invisibleCullingTimer.Stop();

    Timer bufferFillTimer("Buffer filling");

    std::shared_ptr<VertexBuffer> vb =
        std::make_shared<VertexBuffer>(64 * 64 * 64 * 9 * 36 * sizeof(float));
    vb->Bind();

    Model stoneBlock = {0, 0, 0, 0, 0, 0};
    Model dirtBlock = {1, 1, 1, 1, 1, 1};
    Model grassBlock = {2, 2, 2, 2, 3, 1};
    Model glassBlock = {5, 5, 5, 5, 5, 5};

    const std::map<int, Model> models = {
        {1, stoneBlock}, {2, dirtBlock}, {3, grassBlock}, {4, glassBlock}};

    int grassOffset = 0;
    int offset = 0;

    for (int i = 0; i < 2; i++)
    {
        unsigned int localOffset = offset;
        for (const Block& b : visibleBlocks[i])
        {
            glm::vec3 pos = b.position;
            Model m = models.at(b.model);

            if (b.facesVisible[0]) // face 1
            {
                float face[6][12] = {
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.front}, // 2
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.front}, // 0
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     -1.0, 0.0, 0.0, 1.0f, 1.0f, (float)m.front}, // 1
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.front}, // 2
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 0.0f, (float)m.front}, // 4
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     -1.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.front} // 0
                };
                memcpy(buffer + localOffset, face, 6 * 12 * sizeof(float));
                localOffset += 6 * 12 * sizeof(float);
            }
            if (b.facesVisible[1]) // face 2 (bottom)
            {
                float face[6][12] = {
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f, 1.0f, 1.0f, 1.0f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.bottom}, // 5
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f, 1.0f, 1.0f, 1.0f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.bottom}, // 0
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f, 1.0f, 1.0f, 1.0f,
                     0.0, -1.0, 0.0, 1.0f, 1.0f, (float)m.bottom}, // 6
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f, 1.0f, 1.0f, 1.0f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.bottom}, // 5
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f, 1.0f, 1.0f, 1.0f,
                     0.0, 0.0, 0.0, 0.0f, 0.0f, (float)m.bottom}, // 1
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f, 1.0f, 1.0f, 1.0f,
                     0.0, -1.0, 0.0, 0.0f, 1.0f, (float)m.bottom}, // 0
                };
                memcpy(buffer + localOffset, face, 6 * 12 * sizeof(float));
                localOffset += 6 * 12 * sizeof(float);
            }
            if (b.facesVisible[2]) // face 3
            {
                float face[6][12] = {
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.left}, // 3
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.left}, // 0
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, -1.0, 0.0f, 0.0f, (float)m.left}, // 4
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.left}, // 0
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.left}, // 3
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, -1.0, 1.0f, 1.0f, (float)m.left}, // 6
                };
                memcpy(buffer + localOffset, face, 6 * 12 * sizeof(float));
                localOffset += 6 * 12 * sizeof(float);
            }
            if (b.facesVisible[3]) // face 4
            {
                float face[6][12] = {
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.right}, // 7
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.right}, // 6
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     1.0, 0.0, 0.0, 0.0f, 0.0f, (float)m.right}, // 3
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.right}, // 6
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.right}, // 7
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     1.0, 0.0, 0.0, 1.0f, 1.0f, (float)m.right}, // 5
                };
                memcpy(buffer + localOffset, face, 6 * 12 * sizeof(float));
                localOffset += 6 * 12 * sizeof(float);
            }
            if (b.facesVisible[4]) // face 5
            {
                float face[6][12] = {
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, 0}, // 7
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 1.0f, 0}, // 3
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 1.0, 0.0, 0.0f, 1.0f, (float)m.top}, // 4
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, 0}, // 7
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 0.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, 0}, // 4
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 1.0, 0.0, 0.0f, 0.0f, (float)m.top}, // 2
                };
                memcpy(buffer + localOffset, face, 6 * 12 * sizeof(float));
                localOffset += 6 * 12 * sizeof(float);
            }
            if (b.facesVisible[5]) // face 6
            {
                float face[6][12] = {
                    {pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 1.0f, 0.0f, (float)m.back}, // 7
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 0.0f, (float)m.back}, // 2
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 1.0, 1.0f, 1.0f, (float)m.back}, // 5
                    {pos.x + 0.0f, pos.y + 1.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 0.0f, (float)m.back}, // 2
                    {pos.x + 0.0f, pos.y + 0.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 0.0, 0.0f, 1.0f, (float)m.back}, // 1
                    {pos.x + 1.0f, pos.y + 0.0f, pos.z + 1.0f, 0.6f, 0.8f, 0.4f,
                     0.0, 0.0, 1.0, 1.0f, 1.0f, (float)m.back}, // 5
                };
                memcpy(buffer + localOffset, face, 6 * 12 * sizeof(float));
                localOffset += 6 * 12 * sizeof(float);
            }
        }
        offset = localOffset;

        if (i == 0)
        {
            grassOffset = offset;
        }
    }
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, offset, buffer));

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);

    m_VertexArray.AddBuffer(vb, layout);

    bufferFillTimer.Stop();

    m_NumVertices = offset / (12 * sizeof(float));
}