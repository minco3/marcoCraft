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

#define STB_IMAGE_IMPLEMENTATION
#include "includes/textures/stb_image.h"

#include "includes/opengl/renderer.h"
#include "includes/opengl/VertexBuffer.h"
#include "includes/opengl/VertexBufferLayout.h"
#include "includes/opengl/VertexArray.h"
#include "includes/opengl/IndexBuffer.h"
#include "includes/opengl/Shader.h"

#include "includes/objects/camera/camera.h"
#include "includes/objects/text/Text.h"

#include "includes/primitives/Quad.h"

int main(int argc, char** argv) {

    const int width = 1000, height = 1000;

    bool mouseVisible = true, lastMouseVisible = true;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_Surface* icon;
    SDL_RWops*  rwop;
    rwop = SDL_RWFromFile("res/marcotriangle.png", "rb");
    icon = IMG_LoadPNG_RW(rwop);
    SDL_SetWindowIcon(window, icon);


    glewInit();

    {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    Shader SimpleShader("../res/shaders/SimpleShader.glsl");
    Shader TextShader("../res/shaders/TextShader.glsl");
    Shader FlatShader("../res/shaders/FlatShader.glsl");

    Font font("../res/arial.ttf", 48);

    Quad quad;

    quad.UpdateVertices(glm::vec2(100, 100), glm::vec2(100, 100));

    // Text fpsCounter(font);

    // loadFont("../includes/text/arial.ttf", 48, characters);

    static const GLfloat vertices[8][6] = {
        { -1.0f,-1.0f,-1.0f,    1.0f,  0.0f,  0.0f }, //0
        { -1.0f,-1.0f, 1.0f,    0.0f,  1.0f,  0.0f }, //1
        { -1.0f, 1.0f, 1.0f,    0.0f,  0.0f,  1.0f }, //2
        { 1.0f, 1.0f,-1.0f,     1.0f,  1.0f,  0.0f }, //3
        { -1.0f, 1.0f,-1.0f,    0.0f,  1.0f,  1.0f }, //4
        { 1.0f,-1.0f, 1.0f,     1.0f,  0.0f,  1.0f }, //5
        { 1.0f,-1.0f,-1.0f,     0.5f,  1.0f,  0.5f }, //6
        { 1.0f, 1.0f, 1.0f,     1.0f,  0.5f,  0.5f }, //7
    };

    static const unsigned int indices[36] {
        0, 1, 2,
        3, 0, 4,
        5, 0, 6,
        3, 6, 0,
        0, 2, 4,
        5, 1, 0,
        2, 1, 5,
        7, 6, 3,
        6, 7, 5, 
        7, 3, 4,
        7, 4, 2,
        7, 2, 5
    };
    
    VertexArray va;
    va.Bind();
    VertexBuffer vb(vertices, 8*6*sizeof(float));
    VertexBufferLayout(layout);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 36);

    va.Unbind();

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


    SDL_Event event;
    bool running = true, debug_fps = true;

    int fpsProfileFrame = 0;
    std::stringstream fpsCount;

    double lastTime = SDL_GetTicks64(), currentTime = lastTime;

    Camera camera;
    camera.SetScreenSize(width, height);

    std::chrono::high_resolution_clock::time_point p1, p2;
    std::chrono::duration<double> duration;

    while (running) {
        lastTime = currentTime;
        currentTime = SDL_GetTicks64();
        float deltaTime = float(currentTime - lastTime);
        glm::vec3 cameraVelocity;

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


        SimpleShader.Bind();
        SimpleShader.SetUniformMat4fv("MVP", camera.getMVP());
        
        va.Bind();
        // ib.Bind();
        GLCall(glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, 0));

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        TextShader.Bind();
        TextShader.SetUniform3f("textColor", glm::vec3(1.0f,1.0f,1.0f));
        TextShader.SetUniformMat4fv("projection", glm::ortho(0.0f, 1000.0f, 0.0f, 1000.0f));
        font.Bind();
        Character c = font.m_Characters.at('M');
        
        TextShader.SetUniform1i("text", 0);
        quad.GetVertexArray().Bind();
        GLCall(glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0));
        glBlendFunc(GL_ONE, GL_ZERO);


        if (debug_fps) {
            if (fpsProfileFrame % 128 == 0) {
                fpsProfileFrame = 1;
                p1 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>> (p1-p2);
                fpsCount.clear();
                fpsCount.str(std::string());
                fpsCount << std::fixed << std::setprecision(1) << 1/duration.count();
            } else {
                fpsProfileFrame++;
            }
            // fpsCounter.RenderText();
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

