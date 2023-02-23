#include "includes/application/Application.h"
#include "Game.h"

#include <filesystem>
#include <random>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image/stb_image.h"

Game::Game()
    : running(true), m_TextureArray(GL_RGBA), grassOffset(0), offset(0), fullscreen(false), m_Font("res/arial.ttf", 48),
    m_FpsCounter(std::make_shared<Font>(m_Font)), debug_fps(true), mouseVisible(true), m_Instance(&Application::Get()), allocated(1024), allocator(1024*1000*1000) /*1024M*/
{
    m_FrameBuffer.Unbind();

    m_FrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RGBA);
    m_FrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT1, GL_RGB);
    m_FrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT2, GL_RGB);
    m_FrameBuffer.AddAttachment(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
    m_FrameBuffer.CheckFrameBuffer();

    m_SSAOFrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RED);
    m_SSAOFrameBuffer.CheckFrameBuffer();

    m_SSAOBlurFrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RED);
    m_SSAOBlurFrameBuffer.CheckFrameBuffer();


    buffer = allocator.alloc(512*1000*1000);

    std::vector<glm::vec3> ssaoNoise;
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (int i=0; i<64; i++)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = float(i) / 64.0;
        sample *= std::lerp(0.1f, 1.0f, scale * scale);
        samples.push_back(sample);

    }
    for (int i=0; i<16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0);
        ssaoNoise.push_back(noise);
    }

    GLCall(glGenTextures(1, &texNoise));
    GLCall(glBindTexture(GL_TEXTURE_2D, texNoise));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data()));


    m_FpsCounter.setPosition(glm::vec2(100, m_Instance->m_Height-100));
    m_FpsCounter.SetScreenSize(m_Instance->m_Width, m_Instance->m_Height);

    m_ScreenQuad.UpdateBuffer(glm::vec2(-1,-1), glm::vec2(2, 2), glm::vec2(0,1), glm::vec2(1,-1));

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

    for (int x=-1; x<=1; x++)
    {
        for (int z=-1; z<=1; z++)
        {
            m_World.GenerateChunk({x,z});
        }
    }

    std::vector<std::shared_ptr<Cube>> solidBlocks, grassBlocks, transparentBlocks;

    std::vector<Block> visibleBlocks[2];

    Timer invisibleCullingTimer("Invisible face culling");

    for (const Chunk& chunk : m_World.Chunks())
    {
        for (int x=0; x<chunk.data.size(); x++)
        {
            for (int y=0; y<chunk.data[x].size(); y++)
            {
                for (int z=0; z<chunk.data[x][y].size(); z++)
                {
                    bool isVisible = false; 
                    std::array<bool, 6> faces{false};

                    if (!chunk.data[x][y][z]) continue;
                    
                    if (x==0 || !chunk.data[x-1][y][z])
                    {
                        isVisible = true;
                        faces[0] = true;
                    }
                    if (y==0 || !chunk.data[x][y-1][z])
                    {
                        isVisible = true;
                        faces[1] = true;
                    }
                    if (z==0 || !chunk.data[x][y][z-1])
                    {
                        isVisible = true;
                        faces[2] = true;
                    }
                    if (x==chunk.data.size()-1 || !chunk.data[x+1][y][z])
                    {
                        isVisible = true;
                        faces[3] = true;
                    }
                    if (y==chunk.data[0].size()-1 || !chunk.data[x][y+1][z])
                    {
                        isVisible = true;
                        faces[4] = true;
                    }
                    if (z==chunk.data[0][0].size()-1 || !chunk.data[x][y][z+1])
                    {
                        isVisible = true;
                        faces[5] = true;
                    }
                    
                    if (isVisible)
                    {
                        // grass
                        if (chunk.data[x][y][z] == 3) {
                            visibleBlocks[1].push_back({faces, {chunk.pos.x*16+x,y,chunk.pos.y*16+z}, chunk.data[x][y][z]});
                        }
                        else
                        {
                            visibleBlocks[0].push_back({faces, {chunk.pos.x*16+x,y,chunk.pos.y*16+z}, chunk.data[x][y][z]});
                        }
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

    std::filesystem::path rootdir = std::filesystem::current_path();
    
    std::filesystem::path shaderdir = std::filesystem::current_path() += "/res/shaders";

    for (auto const& entry : std::filesystem::directory_iterator(shaderdir))
    {
        std::cout << "loading shader " << entry.path().stem().string() << std::endl;
        m_ShaderLibrary.Load(entry.path().string());
    }
    
    std::filesystem::path texturedir = std::filesystem::current_path() += "/res/texture";
    std::filesystem::directory_iterator texturedir_iter (texturedir);

    int textureCount = std::distance(texturedir_iter, {}); // count the number of textures to load

    m_TextureArray.Resize(glm::ivec3(16,16,textureCount));
    for (auto const& entry : std::filesystem::directory_iterator(texturedir))
    {
        int x, y, bits;
        std::cout << "loading texture " << entry.path().stem().string() << std::endl;
        unsigned char* data = stbi_load(entry.path().string().c_str(), &x, &y, &bits, 4);
        m_TextureArray.SetData(glm::vec2(0, 0), textureID.at(entry.path().stem().string()), glm::vec2(16,16), data);
    }

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    m_Camera.SetScreenSize(Application::Get().m_Width, Application::Get().m_Height);
    m_Camera.setPosition(glm::vec3(0, 65, 0));

    std::chrono::high_resolution_clock::time_point p1, p2;
    std::chrono::duration<double> duration;

    Timer bufferFillTimer("Buffer filling");

    m_VertexArray.Bind();

    std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(64*64*64*9*36*sizeof(float));
    vb->Bind();

    for (int i=0; i<2; i++)
    {
        unsigned int localOffset = offset;
        for (const Block& b : visibleBlocks[i])
        {
            glm::vec3 pos = b.position;
            Model m = models.at(b.model);        

            if (b.facesVisible[0]) // face 1
            {
                float face [6][12] = {  
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.front }, //2
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.front }, //0
                    { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,   0.6f, 0.8f, 0.4f,  -1.0,  0.0,  0.0,   1.0f, 1.0f, (float)m.front }, //1
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.front }, //2
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   0.0f, 0.0f, (float)m.front }, //4
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,  -1.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.front } //0
                };
                memcpy(buffer+localOffset, face, 6*12*sizeof(float));
                localOffset+=6*12*sizeof(float);
            }
            if (b.facesVisible[1]) // face 2 (bottom)
            {
                float face [6][12] = {
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,   1.0f, 1.0f, 1.0f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.bottom }, //5
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,   1.0f, 1.0f, 1.0f,   0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.bottom }, //0
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,   1.0f, 1.0f, 1.0f,   0.0, -1.0,  0.0,   1.0f, 1.0f, (float)m.bottom }, //6
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,   1.0f, 1.0f, 1.0f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.bottom }, //5
                    { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,   1.0f, 1.0f, 1.0f,   0.0,  0.0,  0.0,   0.0f, 0.0f, (float)m.bottom }, //1
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,   1.0f, 1.0f, 1.0f,   0.0, -1.0,  0.0,   0.0f, 1.0f, (float)m.bottom }, //0
                };
                memcpy(buffer+localOffset, face, 6*12*sizeof(float));
                localOffset+=6*12*sizeof(float);
            }
            if (b.facesVisible[2]) // face 3
            {
                float face [6][12] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.left }, //3
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.left }, //0
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0, -1.0,   0.0f, 0.0f, (float)m.left }, //4
                    { pos.x+0.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.left }, //0
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.left }, //3
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0, -1.0,   1.0f, 1.0f, (float)m.left }, //6
                };
                memcpy(buffer+localOffset, face, 6*12*sizeof(float));
                localOffset+=6*12*sizeof(float);

            }
            if (b.facesVisible[3]) // face 4
            {
                float face [6][12] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.right }, //7
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.right }, //6
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   1.0,  0.0,  0.0,   0.0f, 0.0f, (float)m.right }, //3
                    { pos.x+1.0f, pos.y+0.0f, pos.z+0.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.right }, //6
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,   0.6f, 0.8f, 0.4f,   0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.right }, //7
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,   0.6f, 0.8f, 0.4f,   1.0,  0.0,  0.0,   1.0f, 1.0f, (float)m.right }, //5
                }; 
                memcpy(buffer+localOffset, face, 6*12*sizeof(float));
                localOffset+=6*12*sizeof(float);

            }
            if (b.facesVisible[4]) // face 5
            {
                float face [6][12] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   1.0f, 0.0f, 0 }, //7
                    { pos.x+1.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   1.0f, 1.0f, 0 }, //3
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0,  1.0,  0.0,   0.0f, 1.0f, (float)m.top }, //4
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   1.0f, 0.0f, 0 }, //7
                    { pos.x+0.0f, pos.y+1.0f, pos.z+0.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   0.0f, 1.0f, 0 }, //4
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  1.0,  0.0,   0.0f, 0.0f, (float)m.top }, //2
                };
                memcpy(buffer+localOffset, face, 6*12*sizeof(float));
                localOffset+=6*12*sizeof(float);

            }
            if (b.facesVisible[5]) // face 6
            {
                float face [6][12] = {
                    { pos.x+1.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   1.0f, 0.0f, (float)m.back }, //7
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   0.0f, 0.0f, (float)m.back }, //2
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  1.0,   1.0f, 1.0f, (float)m.back }, //5
                    { pos.x+0.0f, pos.y+1.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   0.0f, 0.0f, (float)m.back }, //2
                    { pos.x+0.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  0.0,   0.0f, 1.0f, (float)m.back }, //1
                    { pos.x+1.0f, pos.y+0.0f, pos.z+1.0f,  0.6f, 0.8f, 0.4f,  0.0,  0.0,  1.0,   1.0f, 1.0f, (float)m.back }, //5
                };
                memcpy(buffer+localOffset, face, 6*12*sizeof(float));
                localOffset+=6*12*sizeof(float);
            }            
        }
        offset = localOffset;

        if (i == 0)
        {
            grassOffset = offset;
        }
    }
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, offset, buffer));


    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);
    layout.Push(GL_FLOAT, 3);

    m_VertexArray.AddBuffer(vb, layout);

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
                SDL_SetWindowFullscreen(Application::Get().m_Window, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
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
    m_Camera.move(Application::Get().m_LastFrameTime);

}

void Game::Draw()
{
    GLCall(glEnable(GL_DEPTH_TEST));

    m_FrameBuffer.BindBuffers();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_TextureArray.Bind();

    m_VertexArray.Bind();

    std::shared_ptr<Shader> GrassShader = m_ShaderLibrary.Get("GrassShader");
    GrassShader->Bind();
    GrassShader->SetUniformMat4fv("MVP", m_Camera.getMVP());
    GrassShader->SetUniform1i("textureSlot", 1);
    GLCall(glDrawArrays(GL_TRIANGLES, grassOffset/(12*sizeof(float)), (offset-grassOffset)/(12*sizeof(float))));

    std::shared_ptr<Shader> CubeShader = m_ShaderLibrary.Get("CubeShader");
    CubeShader->Bind();
    CubeShader->SetUniformMat4fv("MVP", m_Camera.getMVP());
    CubeShader->SetUniform1i("textureSlot", 1);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, grassOffset/(12*sizeof(float))));


    GLCall(glDisable(GL_DEPTH_TEST));

    m_ScreenQuad.Bind();
    
    m_FrameBuffer.BindTexture(GL_TEXTURE0, GL_COLOR_ATTACHMENT0);
    m_FrameBuffer.BindTexture(GL_TEXTURE1, GL_COLOR_ATTACHMENT1);
    m_FrameBuffer.BindTexture(GL_TEXTURE2, GL_COLOR_ATTACHMENT2);
    m_FrameBuffer.BindTexture(GL_TEXTURE3, GL_DEPTH_ATTACHMENT);

    glActiveTexture(GL_TEXTURE4);
    GLCall(glBindTexture(GL_TEXTURE_2D, texNoise));

    std::shared_ptr<Shader> SSAOShader = m_ShaderLibrary.Get("SSAOShader");
    SSAOShader->Bind();
    SSAOShader->SetUniform1i("gNormal", 1);
    SSAOShader->SetUniform1i("gPosition", 2);
    SSAOShader->SetUniform1i("texNoise", 4);
    SSAOShader->SetUniformMat4fv("Projection", glm::perspective(glm::radians(45.0f), (float)m_Instance->m_Width / (float)m_Instance->m_Height, 0.1f, 100.0f));
    for (int i=0; i<64; i++)
    {
        SSAOShader->SetUniform3f("samples[" + std::to_string(i) + "]", samples[i]);
    }

    m_SSAOFrameBuffer.BindBuffers();
    GLCall(glDrawElements(GL_TRIANGLES, m_ScreenQuad.IndexCount(), GL_UNSIGNED_INT, nullptr));
    m_SSAOFrameBuffer.BindTexture(GL_TEXTURE4, GL_COLOR_ATTACHMENT0);
    m_SSAOFrameBuffer.Unbind();


    std::shared_ptr<Shader> SSAOBlurShader = m_ShaderLibrary.Get("SSAOBlurShader");
    SSAOBlurShader->Bind();
    SSAOBlurShader->SetUniform1i("ssaoTexture", 4);

    m_SSAOBlurFrameBuffer.BindBuffers();
    GLCall(glDrawElements(GL_TRIANGLES, m_ScreenQuad.IndexCount(), GL_UNSIGNED_INT, nullptr));
    m_SSAOBlurFrameBuffer.BindTexture(GL_TEXTURE4, GL_COLOR_ATTACHMENT0);
    m_SSAOBlurFrameBuffer.Unbind();


    std::shared_ptr<Shader> ScreenShader = m_ShaderLibrary.Get("ScreenShader");
    ScreenShader->Bind();
    ScreenShader->SetUniform1i("gAlbedo", 0);
    ScreenShader->SetUniform1i("gNormal", 1);
    ScreenShader->SetUniform1i("gPosition", 2);
    ScreenShader->SetUniform1i("gDepth", 3);
    ScreenShader->SetUniform1i("ssaoTexture", 4);

    GLCall(glDrawElements(GL_TRIANGLES, m_ScreenQuad.IndexCount(), GL_UNSIGNED_INT, nullptr));

    if (debug_fps)
    {
        using namespace std::chrono_literals;
        if (std::chrono::high_resolution_clock::now() > p1+500ms) {
            std::stringstream fpsCount;
            p1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>> (p1-p2);
            fpsCount.clear();
            fpsCount.str(std::string());
            fpsCount << std::fixed << std::setprecision(0) << 1/duration.count();
            m_FpsCounter.SetString(fpsCount.str() + " fps");
        }
        m_FpsCounter.RenderText();
    }

    p2 = std::chrono::high_resolution_clock::now();
    SDL_GL_SwapWindow(Application::Get().m_Window);
}
