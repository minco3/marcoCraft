#include "Chunk.h"

Chunk::Chunk(SimplexNoise* gen, glm::ivec2 _pos)
    : pos(_pos)
{
    for (int x=0; x<data.size(); x++)
    {
        for (int z=0; z<data[x][0].size(); z++)
        {
            float val = gen->fractal(16, (pos.x*16+x)/100.f, (pos.y*16+z)/100.f);
            int height = 64 + val*20;
            for (int y=0; y<data[x].size(); y++)
            {
                if (y <= height-2)
                {
                    data[x][y][z] = 1;
                }
                else if (y == height-1)
                {
                    data[x][y][z] = 2;
                }
                else if (y == height)
                {
                    data[x][y][z] = 3;
                }
                else
                {
                    data[x][y][z] = 0;
                }
            }
        }
    }
}