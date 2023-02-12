#pragma once

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

#include "includes/objects/camera/Camera.h"
#include "includes/objects/text/Text.h"

#include "includes/primitives/TexturedQuad.h"
#include "includes/primitives/TexturedCube.h"

#include "includes/utils/Timer.h"

class Application
{
public:
    Application();

    void Run();

    static Application& Get() { return *s_Instance; }

private:
    static Application* s_Instance;

    
};