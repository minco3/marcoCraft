#pragma once

class Application;
class Camera;
class TextureArray;
class VertexArray;
class VertexBuffer;
class FrameCounter;

#include "World.h"

class Game {
public:

Game();

void Event();
void Update();
void Draw();

bool isRunning() {return running;}

private:
    bool running;
    // Application& m_Instance;
    Camera m_Camera;

    VertexArray m_VertexArray;
    TextureArray m_TextureArray;
    ShaderLibrary m_ShaderLibrary;

    World m_World;

    int grassOffset;
    int offset;

    FrameCounter fpsCounter;

    bool mouseVisible;
    bool fullscreen;
    bool debug_fps;
};