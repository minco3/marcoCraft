#pragma once

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

#include "external/SimplexNoise/SimplexNoise.h"

#include "includes/opengl/Renderer.h"
#include "includes/opengl/VertexBuffer.h"
#include "includes/opengl/VertexBufferLayout.h"
#include "includes/opengl/VertexArray.h"
#include "includes/opengl/IndexBuffer.h"
#include "includes/opengl/Shader.h"
#include "includes/opengl/Texture2D.h"
#include "includes/opengl/TextureArray.h"
#include "includes/opengl/FrameBuffer.h"

#include "includes/objects/camera/Camera.h"
#include "includes/objects/text/Text.h"
#include "includes/objects/frameCounter/FrameCounter.h"

#include "includes/primitives/TexturedQuad.h"
#include "includes/primitives/TexturedCube.h"

#include "includes/utils/Timer.h"

#include "includes/game/Game.h"


class Application
{
friend class Game; // could maybe do this better
public:
    static Application& Get() { return *s_Instance; }

    Application();
    ~Application();
    void Run();

    glm::uvec2 WindowSize() { return glm::uvec2(m_Width, m_Height); }

private:

    static Application* s_Instance;

    unsigned int m_Width;
    unsigned int m_Height;
    float m_LastFrameTime;
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    SDL_GLContext m_Context;

};