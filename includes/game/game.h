#pragma once

class Application;
class Camera;
class TextureArray;
class VertexArray;
class VertexBuffer;
class FrameBuffer;
class Quad;
class Font;
class Text;

#include <chrono>

#include "World.h"
#include "includes/utils/Alloc.h"

class Game {
public:

Game();

void Event();
void Update();
void Draw();

bool isRunning() {return running;}

private:
    bool running;
    
    Application* m_Instance;
    
    Camera m_Camera;

    VertexArray m_VertexArray;
    TextureArray m_TextureArray;
    ShaderLibrary m_ShaderLibrary;

    World m_World;

    int grassOffset;
    int offset;

    std::chrono::high_resolution_clock::time_point p1, p2;

    Font m_Font;
    Text m_FpsCounter;

    FrameBuffer m_FrameBuffer;
    Quad m_ScreenQuad;

    bool mouseVisible;
    bool fullscreen;
    bool debug_fps;

    int allocated;
    Alloc allocator;
    void* buffer;
};