#include "includes/application/Application.h"
#include "Game.h"


#include <filesystem>
#include "external/stb_image/stb_image.h"

Game::Game()
    : running(true), m_Instance(Application::Get())
{

    SimplexNoise worldgen;

    std::map<std::string, int> textureID 
    {
        {"stone", 0},
        {"dirt", 1},
        {"grass_block_side", 2},
        {"grass_block_top", 3},
        {"grass_block_side_overlay", 4},
        {"glass", 5},
        {"oak_leaves", 6}
    };

    Model stoneBlock = {0,0,0,0,0,0};
    Model dirtBlock  = {1,1,1,1,1,1};
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

    Shader SimpleShader("res/shaders/SimpleShader.glsl");
    Shader FlatShader("res/shaders/FlatShader.glsl");
    Shader CubeShader("res/shaders/CubeShader.glsl");
    Shader GrassShader("res/shaders/GrassShader.glsl");
    Shader TransparentShader("res/shaders/TransparentShader.glsl");

    Font font("res/arial.ttf", 48);

    std::filesystem::path rootdir = std::filesystem::current_path() += "/res/texture";
    std::filesystem::directory_iterator rootdir_iter (rootdir);

    int textureCount = std::distance(rootdir_iter, {}); // count the number of textures to load


    TextureArray textureArray(GL_RGBA);
    textureArray.Resize(glm::vec3(16,16,textureCount));
    for (auto const& entry : std::filesystem::directory_iterator(rootdir))
    {
        int x, y, bits;
        // std::cout << "loading texture " << entry.path().stem().string() << std::endl;
        unsigned char* data = stbi_load(entry.path().string().c_str(), &x, &y, &bits, 4);
        textureArray.SetData(glm::vec2(0, 0), textureID.at(entry.path().stem().string()), glm::vec2(16,16), data);
    }

    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    // GLCall(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 2));
    // GLCall(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));

    Text fpsCounter(font);
    fpsCounter.setPosition(glm::vec2(100, m_Instance.m_Height-100));
    fpsCounter.SetScreenSize(m_Instance.m_Width, m_Instance.m_Height);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    SDL_Event event;
    bool running = true, debug_fps = true, fullscreen = false;

    int fpsProfileFrame = 0;
    std::stringstream fpsCount;

    double lastTime = SDL_GetTicks64(), currentTime = lastTime;

    m_Camera.SetScreenSize(m_Instance.m_Width, m_Instance.m_Height);
    m_Camera.setPosition(glm::vec3(0, 65, 0));

    std::chrono::high_resolution_clock::time_point p1, p2;
    std::chrono::duration<double> duration;

    Timer bufferFillTimer("Buffer filling");

    m_va.Bind();

    m_vb(128*64*128*9*36*sizeof(float));
    m_vb.Bind();
    
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

    m_va.AddBuffer(vb, layout);

    bufferFillTimer.Stop();

}

void Game::Event()
{
    SDL_Event event;
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
            case SDL_WINDOWEVENT_RESIZED:
                GLCall(glViewport(0, 0, event.window.data1, event.window.data2));
                std::cout << event.window.data1 << " " << event.window.data2 << '\n';
                // m_Camera.SetScreenSize(event.window.data1, event.window.data1);
                // fpsCounter.SetScreenSize(event.window.data1, event.window.data2);
                break;
            case SDL_WINDOW_MAXIMIZED:
                std::cout << "maximized\n";
            }
        break;
        case SDL_KEYDOWN:
        if (event.key.repeat) break;
            switch(event.key.keysym.sym) {
            case SDLK_w:
                m_Camera.forward = true;
                break;
            case SDLK_d:
                m_Camera.right = true;
                break;
            case SDLK_s:
                m_Camera.back = true;
                break;
            case SDLK_a:
                m_Camera.left = true;
                break;
            case SDLK_SPACE:
                m_Camera.up = true;
                break;
            case SDLK_c:
                m_Camera.down = true;
                break;
            case SDLK_LSHIFT:
                m_Camera.sprint = true;
                break;
            case SDLK_LALT:
                if (!fullscreen)
                {
                    mouseVisible = !mouseVisible;
                    SDL_SetRelativeMouseMode(mouseVisible ?  SDL_FALSE : SDL_TRUE);
                } 
                break;
            case SDLK_F3:
                debug_fps = !debug_fps;
                break;
            case SDLK_F11:
                SDL_SetWindowFullscreen(m_Instance.m_Window, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                fullscreen = !fullscreen;
                break;
            }
        break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym) {
            case SDLK_w:
                m_Camera.forward = false;
                break;
            case SDLK_d:
                m_Camera.right = false;
                break;
            case SDLK_s:
                m_Camera.back = false;
                break;
            case SDLK_a:
                m_Camera.left = false;
                break;
            case SDLK_SPACE:
                m_Camera.up = false;
                break;
            case SDLK_c:
                m_Camera.down = false;
                break;
            case SDLK_LSHIFT:
                m_Camera.sprint = false;
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            if (!mouseVisible) {
                m_Camera.look(event.motion.xrel, event.motion.yrel);
            }
            break;
        }
    }
}

void Game::Update()
{
    m_Camera.move(m_Instance.m_LastFrameTime);

}

void Game::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_TextureArray.Bind();

    m_va.Bind();

    CubeShader.Bind();
    CubeShader.SetUniformMat4fv("MVP", camera.getMVP());
    CubeShader.SetUniform1i("textureSlot", 1);

    GLCall(glDrawArrays(GL_TRIANGLES, 0, grassOffset/(9*sizeof(float))));

    GrassShader.Bind();
    GrassShader.SetUniformMat4fv("MVP", camera.getMVP());
    GrassShader.SetUniform1i("textureSlot", 1);

    GLCall(glDrawArrays(GL_TRIANGLES, grassOffset/(9*sizeof(float)), (offset-grassOffset)/(9*sizeof(float))));

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
