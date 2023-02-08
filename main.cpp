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
#include "includes/textures/stb_image.h"

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

int main(int argc, char** argv) {

    const int width = 1000, height = 1000;

    bool mouseVisible = true, lastMouseVisible = true;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1); // disable vsync

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

    std::vector<std::shared_ptr<Cube>> solidBlocks, grassBlocks, transparentBlocks;


    typedef std::array<std::array<std::array<int, 16>, 16>, 16> chunk;

    std::array<std::array<chunk, 4>, 4> world;

    for (int i=0; i<world.size(); i++)
    {
        for (int j=0; j<world[i].size(); j++)
        {
            for (int x=0; x<16; x++)
            {
                for (int z=0; z<16; z++)
                {  
                    world[i][j][x][0][z] = 1;
                    world[i][j][x][1][z] = 2;
                    world[i][j][x][2][z] = 3;
                    for (int y=3; y<16; y++)
                    {
                        world[i][j][x][y][z] = 0;
                    }
                }
            }
        }
    }

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
    camera.setPosition(glm::vec3(0, 3, 0));

    std::chrono::high_resolution_clock::time_point p1, p2;
    std::chrono::duration<double> duration;

    for (int i=0; i<world.size(); i++)
    {
        for (int j=0; j<world[i].size(); j++)
        {
            for (int x = 0; x<world[i][j].size(); x++)
            {
                for (int y = 0; y<world[i][j][x].size(); y++)
                {
                    for (int z = 0; z<world[i][j][x][y].size(); z++)
                    {
                        if (!world[i][j][x][y][z]) continue;

                        std::shared_ptr<Cube> c(new Cube);
                        c->UpdateBuffer(glm::vec3(i*16+x, y, j*16+z), glm::vec3(1, 1, 1), glm::vec2(), glm::vec2(), models.at(world[i][j][x][y][z]));

                        if (world[i][j][x][y][z] == 1 || world[i][j][x][y][z] == 2) // stone / dirt
                        {
                            solidBlocks.push_back(std::move(c));
                        }
                        else if (world[i][j][x][y][z] == 3) // grass
                        {
                            grassBlocks.push_back(std::move(c));
                        }
                        else
                        {
                            transparentBlocks.push_back(std::move(c));
                        }
                    }
                }
            }
        }
    }

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

        CubeShader.Bind();
        CubeShader.SetUniformMat4fv("MVP", camera.getMVP());
        CubeShader.SetUniform1i("textureSlot", 1);

        for (const auto& b : solidBlocks)
        {
            b->Bind();
            GLCall(glDrawArrays(GL_TRIANGLES, 0, b->IndexCount()));
        }

        GrassShader.Bind();
        GrassShader.SetUniformMat4fv("MVP", camera.getMVP());
        GrassShader.SetUniform1i("textureSlot", 1);

        for (const auto& b : grassBlocks)
        {
            b->Bind();
            GLCall(glDrawArrays(GL_TRIANGLES, 0, b->IndexCount()));
        }

        TransparentShader.Bind();
        TransparentShader.SetUniformMat4fv("MVP", camera.getMVP());
        TransparentShader.SetUniform1i("textureSlot", 1);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (const auto& b : transparentBlocks)
        {
            b->Bind();
            GLCall(glDrawArrays(GL_TRIANGLES, 0, b->IndexCount()));
        }

        glBlendFunc(GL_ONE, GL_ZERO);

        if (debug_fps) {
            if (fpsProfileFrame % 128 == 0) {
                fpsProfileFrame = 1;
                p1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>> (p1-p2);
                fpsCount.clear();
                fpsCount.str(std::string());
                fpsCount << std::fixed << std::setprecision(1) << 1/duration.count();
                fpsCounter.SetString(fpsCount.str());
            }
            fpsProfileFrame++;
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

