#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>
#include <GL/glew.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image/stb_image.h"
#include "external/SimplexNoise/SimplexNoise.h"

#include "includes/opengl/renderer.h"
#include "includes/opengl/VertexBuffer.h"
#include "includes/opengl/VertexBufferLayout.h"
#include "includes/opengl/VertexArray.h"
#include "includes/opengl/IndexBuffer.h"
#include "includes/opengl/Shader.h"
#include "includes/opengl/Texture2D.h"
#include "includes/opengl/TextureArray.h"

#include "includes/objects/camera/camera.h"
#include "includes/objects/text/Text.h"

#include "includes/primitives/TexturedQuad.h"
#include "includes/primitives/TexturedCube.h"

#include "includes/utils/Timer.h"

int main(int argc, char** argv) {

    SimplexNoise worldgen;

    const int width = 1920, height = 1080;

    bool mouseVisible = true, lastMouseVisible = true;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1); // vsync

    SDL_Surface* icon;
    SDL_RWops*  rwop;
    rwop = SDL_RWFromFile("res/marcotriangle.png", "rb");
    icon = IMG_LoadPNG_RW(rwop);
    SDL_SetWindowIcon(window, icon);


    glewInit();

    {

    Model stoneBlock = {0,0,0,0,0,0};
    Model dirtBlock = {1,1,1,1,1,1};
    Model grassBlock = {2,2,2,2,3,1};
    Model glassBlock = {5,5,5,5,5,5};

    struct Block
    {
        std::array<bool, 6> facesVisible;
        glm::ivec3 position;
        int model;
    };

    std::vector<std::shared_ptr<Cube>> solidBlocks, grassBlocks, transparentBlocks;

    std::array<std::array<std::array<int, 16>, 128>, 16> world;

    Timer worldgenTimer("Worldgen");

    for (int x=0; x<world.size(); x++)
    {
        for (int z=0; z<world[x][0].size(); z++)
        {
            // world[x][y][z] = 0;
            float val = worldgen.fractal(3, x/100.f, z/100.f);
            int height = 64 + val*20;
            for (int y=0; y<world[x].size(); y++)
            {
                if (y <= height-2)
                {
                    world[x][y][z] = 1;
                }
                else if (y == height-1)
                {
                    world[x][y][z] = 2;
                }
                else if (y == height)
                {
                    world[x][y][z] = 3;
                }
                else
                {
                    world[x][y][z] = 0;
                }
            }
        }
    }

    worldgenTimer.Stop();

    std::vector<Block> visibleBlocks[2];

    Timer invisibleCullingTimer("Invisible face culling");

    for (int x=0; x<world.size(); x++)
    {
        for (int y=0; y<world[x].size(); y++)
        {
            for (int z=0; z<world[x][y].size(); z++)
            {
                bool isVisible = false; 
                std::array<bool, 6> faces{false};

                if (!world[x][y][z]) continue;
                
                if (x==0 || !world[x-1][y][z])
                {
                    isVisible = true;
                    faces[0] = true;
                }
                if (y==0 || !world[x][y-1][z])
                {
                    isVisible = true;
                    faces[1] = true;
                }
                if (z==0 || !world[x][y][z-1])
                {
                    isVisible = true;
                    faces[2] = true;
                }
                if (x==world.size()-1 || !world[x+1][y][z])
                {
                    isVisible = true;
                    faces[3] = true;
                }
                if (y==world[0].size()-1 || !world[x][y+1][z])
                {
                    isVisible = true;
                    faces[4] = true;
                }
                if (z==world[0][0].size()-1 || !world[x][y][z+1])
                {
                    isVisible = true;
                    faces[5] = true;
                }
                
                if (isVisible)
                {
                    // grass
                    if (world[x][y][z] == 3) {
                        visibleBlocks[1].push_back({faces, {x,y,z}, world[x][y][z]});
                    }
                    else
                    {
                        visibleBlocks[0].push_back({faces, {x,y,z}, world[x][y][z]});
                    }
                }
            }
        }
    }

    invisibleCullingTimer.Stop();

    const std::map<int, Model> models = {
        {1, stoneBlock}, {2, dirtBlock}, {3, grassBlock}, {4, glassBlock}
    };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Shader SimpleShader("../res/shaders/SimpleShader.glsl");
    Shader FlatShader("../res/shaders/FlatShader.glsl");
    Shader CubeShader("../res/shaders/CubeShader.glsl");
    Shader GrassShader("../res/shaders/GrassShader.glsl");
    Shader TransparentShader("../res/shaders/TransparentShader.glsl");

    Font font("../res/arial.ttf", 48);

    Quad quad;

    Cube cube;

    cube.UpdateBuffer(glm::vec3(), glm::vec3(), glm::vec2(), glm::vec2(), models.at(1));

    int x, y, bits;
    unsigned char* stoneData = stbi_load("../res/stone.png", &x, &y, &bits, 4);
    unsigned char* dirtData = stbi_load("../res/dirt.png", &x, &y, &bits, 4);
    unsigned char* grassSideData = stbi_load("../res/grass_block_side.png", &x, &y, &bits, 4);
    unsigned char* grassTopData = stbi_load("../res/grass_block_top.png", &x, &y, &bits, 4);
    unsigned char* grassOverlayData = stbi_load("../res/grass_block_side_overlay.png", &x, &y, &bits, 4);
    unsigned char* glassData = stbi_load("../res/glass.png", &x, &y, &bits, 4);

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4));

    TextureArray textureArray;
    textureArray.SetInternalFormat(GL_RGBA);
    textureArray.Resize(glm::vec3(16,16,6));
    textureArray.SetData(glm::vec2(0, 0), 0, glm::vec2(16,16), stoneData);
    textureArray.SetData(glm::vec2(0, 0), 1, glm::vec2(16,16), dirtData);
    textureArray.SetData(glm::vec2(0, 0), 2, glm::vec2(16,16), grassSideData);
    textureArray.SetData(glm::vec2(0, 0), 3, glm::vec2(16,16), grassTopData);
    textureArray.SetData(glm::vec2(0, 0), 4, glm::vec2(16,16), grassOverlayData);
    textureArray.SetData(glm::vec2(0, 0), 5, glm::vec2(16,16), glassData);
    GLCall(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

    Text fpsCounter(font);
    fpsCounter.setPosition(glm::vec2(100,height-100));

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    SDL_Event event;
    bool running = true, debug_fps = true;

    int fpsProfileFrame = 0;
    std::stringstream fpsCount;

    double lastTime = SDL_GetTicks64(), currentTime = lastTime;

    Camera camera;
    camera.SetScreenSize(width, height);
    camera.setPosition(glm::vec3(0, 65, 0));

    std::chrono::high_resolution_clock::time_point p1, p2;
    std::chrono::duration<double> duration;

    Timer bufferFillTimer("Buffer filling");

    VertexArray va;
    va.Bind();

    VertexBuffer vb(128*64*128*9*36*sizeof(float));
    vb.Bind();
    
    int offset = 0, grassOffset = 0;

    for (int i=0; i<2; i++)
    {

        for (const Block& b : visibleBlocks[i])
        {

            glm::vec3 pos = b.position;
            Model m = models.at(b.model);        

            float vertices[36][9];
            
            int facesVisible = 0;

            if (b.facesVisible[0]) // face 1
            {
                float face [6][9] = {
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.front }, //2
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.front }, //0
                    { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.front }, //1
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.front }, //2
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.front }, //4
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.front } //0
                };
                memcpy(vertices+facesVisible * 6, face, 6*9*sizeof(float));
                facesVisible++;
            }
            if (b.facesVisible[1]) // face 2 (bottom)
            {
                float face [6][9] = {
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, (float)m.bottom }, //5
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, (float)m.bottom }, //0
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, (float)m.bottom }, //6
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, (float)m.bottom }, //5
                    { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, (float)m.bottom }, //1
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, (float)m.bottom }, //0
                };
                memcpy(vertices+facesVisible * 6, face, 6*9*sizeof(float));
                facesVisible++;
            }
            if (b.facesVisible[2]) // face 3
            {
                float face [6][9] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.left }, //3
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.left }, //0
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.left }, //4
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.left }, //0
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.left }, //3
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.left }, //6
                };
                memcpy(vertices+facesVisible * 6, face, 6*9*sizeof(float));
                facesVisible++;
            }
            if (b.facesVisible[3]) // face 4
            {
                float face [6][9] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.right }, //7
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.right }, //6
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.right }, //3
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.right }, //6
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.right }, //7
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.right }, //5
                };
                memcpy(vertices+facesVisible * 6, face, 6*9*sizeof(float));
                facesVisible++;
            }
            if (b.facesVisible[4]) // face 5
            {
                float face [6][9] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.top }, //7
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.top }, //3
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.top }, //4
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.top }, //7
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.top }, //4
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.top }, //2
                };
                memcpy(vertices+facesVisible * 6, face, 6*9*sizeof(float));
                facesVisible++;
            }
            if (b.facesVisible[5]) // face 6
            {
                float face [6][9] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 0.0f, (float)m.back }, //7
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.back }, //2
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.back }, //5
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 0.0f, (float)m.back }, //2
                    { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0f, 1.0f, (float)m.back }, //1
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  1.0f, 1.0f, (float)m.back }, //5
                };
                memcpy(vertices+facesVisible * 6, face, 6*9*sizeof(float));
                facesVisible++;
            }

            GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, facesVisible * 6 * 9 * sizeof(float), vertices));
            offset += facesVisible * 6 * 9 * sizeof(float);
            
        }
        if (i == 0)
        {
            grassOffset = offset;
        }
    }

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);

    va.AddBuffer(vb, layout);

    bufferFillTimer.Stop();
    

    // for (int x=0; x<world.size(); x++)
    // {
    //     for (int y=0; y<world[x].size(); y++)
    //     {
    //         for (int z=0; z<world[x][y].size(); z++)
    //         {
    //             if (!world[x][y][z]) continue;

    //             std::shared_ptr<Cube> c(new Cube);
    //             c->UpdateBuffer(glm::vec3(x, y, z), glm::vec3(1, 1, 1), glm::vec2(), glm::vec2(), models.at(world[x][y][z]));

    //             if (world[x][y][z] == 1 || world[x][y][z] == 2) // stone / dirt
    //             {
    //                 solidBlocks.push_back(std::move(c));
    //             }
    //             else if (world[x][y][z] == 3) // grass
    //             {
    //                 grassBlocks.push_back(std::move(c));
    //             }
    //             else
    //             {
    //                 transparentBlocks.push_back(std::move(c));
    //             }
    //         }
    //     }
    // }

    while (running) {
        lastTime = currentTime;
        currentTime = SDL_GetTicks64();
        float deltaTime = float(currentTime - lastTime);

        //process all events
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_WINDOWEVENT:
                switch(event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    running = false;
                    break;
                case SDL_WINDOWEVENT_ENTER:
                    mouseVisible = false;
                    break;
                case SDL_WINDOWEVENT_LEAVE:
                    mouseVisible = true;
                    break;
                }
            break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                case SDLK_w:
                    camera.forward = true;
                    break;
                case SDLK_d:
                    camera.right = true;
                    break;
                case SDLK_s:
                    camera.back = true;
                    break;
                case SDLK_a:
                    camera.left = true;
                    break;
                case SDLK_SPACE:
                    camera.up = true;
                    break;
                case SDLK_c:
                    camera.down = true;
                    break;
                case SDLK_LSHIFT:
                    camera.sprint = true;
                    break;
                case SDLK_LALT:
                    mouseVisible = !mouseVisible;
                    break;
                case SDLK_F3:
                    debug_fps = !debug_fps;
                    break;
                }
            break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                case SDLK_w:
                    camera.forward = false;
                    break;
                case SDLK_d:
                    camera.right = false;
                    break;
                case SDLK_s:
                    camera.back = false;
                    break;
                case SDLK_a:
                    camera.left = false;
                    break;
                case SDLK_SPACE:
                    camera.up = false;
                    break;
                case SDLK_c:
                    camera.down = false;
                    break;
                case SDLK_LSHIFT:
                    camera.sprint = false;
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                if (!mouseVisible) {
                    camera.look(event.motion.xrel, event.motion.yrel);
                }
                break;
            }
        }

        if (lastMouseVisible != mouseVisible) {
            SDL_SetRelativeMouseMode(mouseVisible ?  SDL_FALSE : SDL_TRUE);
            lastMouseVisible = mouseVisible;             
        }

        camera.move(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        textureArray.Bind();



        // CubeShader.Bind();
        // CubeShader.SetUniformMat4fv("MVP", camera.getMVP());
        // CubeShader.SetUniform1i("textureSlot", 1);

        // for (const auto& b : solidBlocks)
        // {
        //     b->Bind();
        //     GLCall(glDrawArrays(GL_TRIANGLES, 0, b->IndexCount()));
        // }

        // GrassShader.Bind();
        // GrassShader.SetUniformMat4fv("MVP", camera.getMVP());
        // GrassShader.SetUniform1i("textureSlot", 1);

        // for (const auto& b : grassBlocks)
        // {
        //     b->Bind();
        //     GLCall(glDrawArrays(GL_TRIANGLES, 0, b->IndexCount()));
        // }

        // TransparentShader.Bind();
        // TransparentShader.SetUniformMat4fv("MVP", camera.getMVP());
        // TransparentShader.SetUniform1i("textureSlot", 1);


        va.Bind();

        CubeShader.Bind();
        CubeShader.SetUniformMat4fv("MVP", camera.getMVP());
        CubeShader.SetUniform1i("textureSlot", 1);

        GLCall(glDrawArrays(GL_TRIANGLES, 0, grassOffset/(9*sizeof(float))));

        GrassShader.Bind();
        GrassShader.SetUniformMat4fv("MVP", camera.getMVP());
        GrassShader.SetUniform1i("textureSlot", 1);

        GLCall(glDrawArrays(GL_TRIANGLES, grassOffset/(9*sizeof(float)), (offset-grassOffset)/(9*sizeof(float))));


        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // for (const auto& b : transparentBlocks)
        // {
        //     b->Bind();
        //     GLCall(glDrawArrays(GL_TRIANGLES, 0, b->IndexCount()));
        // }

        // glBlendFunc(GL_ONE, GL_ZERO);

        if (debug_fps) {
            using namespace std::chrono_literals;
            if (std::chrono::high_resolution_clock::now() > p1+500ms) {
                fpsProfileFrame = 1;
                p1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>> (p1-p2);
                fpsCount.clear();
                fpsCount.str(std::string());
                fpsCount << std::fixed << std::setprecision(0) << 1/duration.count();
                fpsCounter.SetString(fpsCount.str() + " fps");
            }
            fpsCounter.RenderText();
        }
        p2 = std::chrono::high_resolution_clock::now();

        
        SDL_GL_SwapWindow(window);
    }

    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

