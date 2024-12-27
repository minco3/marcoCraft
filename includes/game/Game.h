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
#include <vector>

#include "includes/objects/camera/Camera.h"
#include "includes/objects/text/Text.h"
#include "includes/opengl/FrameBuffer.h"
#include "includes/opengl/Shader.h"
#include "includes/opengl/TextureArray.h"
#include "includes/opengl/VertexArray.h"
#include "World.h"
#include "includes/utils/Alloc.h"

#include <glm/common.hpp>

class Game
{
public:
    Game();

    void Event();
    void Update();
    void Draw();

    bool isRunning() { return running; }

private:
    bool running;

    Application* m_Instance;

    Camera m_Camera;

    TextureArray m_TextureArray;
    ShaderLibrary m_ShaderLibrary;

    World m_World;

    std::chrono::high_resolution_clock::time_point p1, p2;

    Font m_Font;
    Text m_FpsCounter;

    FrameBuffer m_FrameBuffer;
    FrameBuffer m_ShadowBuffer;
    FrameBuffer m_SSAOFrameBuffer;
    FrameBuffer m_SSAOBlurFrameBuffer;

    GLenum texNoise;
    std::vector<glm::vec3> samples;

    glm::vec3 m_LightPos;

    Quad m_ScreenQuad;

    bool mouseVisible;
    bool fullscreen;
    bool debug_fps;
    bool m_Occlusion;

    int allocated;
    int m_kernelSize;
    Alloc allocator;
    uint8_t* buffer;
};