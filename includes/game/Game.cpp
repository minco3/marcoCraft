#include "Game.h"
#include "includes/application/Application.h"

#include <cmath>
#include <filesystem>
#include <random>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image/stb_image.h"

Game::Game()
    : running(true), m_TextureArray(GL_RGBA), fullscreen(false),
      m_Font(std::filesystem::path(ASSET_DIR) / "arial.ttf", 48),
      m_FpsCounter(std::make_shared<Font>(m_Font)), debug_fps(true),
      mouseVisible(true), m_Occlusion(true), m_Instance(&Application::Get()),
      allocated(1024), allocator(1024 * 1000 * 1000), m_kernelSize(64) /*1024M*/
{
    m_FrameBuffer.Unbind();

    m_FrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RGBA);   // albedo
    m_FrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT1, GL_RGBA);   // normal
    m_FrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT2, GL_RGB16F); // position
    m_FrameBuffer.AddAttachment(
        GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT); // depth
    m_FrameBuffer.CheckFrameBuffer();

    m_ShadowBuffer.AddAttachment(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT);
    GLCall(glDrawBuffer(GL_NONE));
    m_ShadowBuffer.CheckFrameBuffer();

    m_SSAOFrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RED);
    m_SSAOFrameBuffer.CheckFrameBuffer();

    m_SSAOBlurFrameBuffer.AddAttachment(GL_COLOR_ATTACHMENT0, GL_RED);
    m_SSAOBlurFrameBuffer.CheckFrameBuffer();

    buffer = allocator.alloc(512 * 1000 * 1000);
    m_World.InvisibleCulling(buffer);

    std::vector<glm::vec3> ssaoNoise;
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for (int i = 0; i < 32; i++)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);

        float scale = float(i) / 32.0;
        sample *= std::lerp(0.1f, 1.0f, scale * scale);
        samples.push_back(sample);
    }
    for (int i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0, 0.0);
        ssaoNoise.push_back(noise);
    }

    GLCall(glGenTextures(1, &texNoise));
    GLCall(glBindTexture(GL_TEXTURE_2D, texNoise));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT,
        ssaoNoise.data()));

    m_FpsCounter.setPosition(glm::vec2(100, m_Instance->m_Height - 100));
    m_FpsCounter.SetScreenSize(m_Instance->m_Width, m_Instance->m_Height);

    m_ScreenQuad.UpdateBuffer(
        glm::vec2(-1, -1), glm::vec2(2, 2), glm::vec2(0, 1), glm::vec2(1, -1));

    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glDepthFunc(GL_LESS));
    GLCall(glEnable(GL_BLEND));

    const std::filesystem::path assetdir = ASSET_DIR;

    const std::filesystem::path shaderdir = assetdir / "shaders";

    for (auto const& entry : std::filesystem::directory_iterator(shaderdir))
    {
        std::cout << "loading shader " << entry.path().stem().string()
                  << std::endl;
        m_ShaderLibrary.Load(entry.path().string());
    }

    std::unordered_map<std::string, int> textureID{
        {"stone", 0},
        {"dirt", 1},
        {"grass_block_side", 2},
        {"grass_block_top", 3},
        {"grass_block_side_overlay", 4},
        {"glass", 5},
        {"oak_leaves", 6},
        {"sun", 7}};

    const std::filesystem::path texturedir = assetdir / "texture";
    std::filesystem::directory_iterator texturedir_iter(texturedir);

    int textureCount = std::distance(
        texturedir_iter, {}); // count the number of textures to load

    m_TextureArray.Resize(glm::ivec3(16, 16, textureCount));
    for (auto const& entry : std::filesystem::directory_iterator(texturedir))
    {
        const std::string& textureName = entry.path().stem().string();
        if (!textureID.contains(textureName))
            continue;

        int x, y, bits;
        std::cout << "loading texture " << textureName
                  << std::endl;
        unsigned char* data =
            stbi_load(entry.path().string().c_str(), &x, &y, &bits, 4);
        m_TextureArray.SetData(
            glm::vec2(0, 0), textureID.at(textureName),
            glm::vec2(16, 16), data);
    }

    GLCall(glClearColor(0.0f, 0.0f, 0.4f, 0.0f));

    m_Camera.SetScreenSize(
        Application::Get().m_Width, Application::Get().m_Height);
    m_Camera.setPosition(glm::vec3(0, 65, 10));

    std::chrono::high_resolution_clock::time_point p1, p2;
    std::chrono::duration<double> duration;
}

void Game::Event()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
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
                GLCall(
                    glViewport(0, 0, event.window.data1, event.window.data2));
                std::cout << event.window.data1 << " " << event.window.data2
                          << '\n';
                // m_Camera.SetScreenSize(event.window.data1,
                // event.window.data1);
                // fpsCounter.SetScreenSize(event.window.data1,
                // event.window.data2);
                break;
            case SDL_WINDOW_MAXIMIZED:
                std::cout << "maximized\n";
            }
            break;
        case SDL_KEYDOWN:
            if (event.key.repeat)
                break;
            switch (event.key.keysym.sym)
            {
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
                    SDL_SetRelativeMouseMode(
                        mouseVisible ? SDL_FALSE : SDL_TRUE);
                }
                break;
            case SDLK_F3:
                debug_fps = !debug_fps;
                break;
            case SDLK_F11:
                SDL_SetWindowFullscreen(
                    Application::Get().m_Window,
                    fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
                fullscreen = !fullscreen;
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
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
            case SDLK_o:
                m_Occlusion = !m_Occlusion;
                break;
            }
            break;
        case SDL_MOUSEMOTION:
            if (!mouseVisible)
            {
                m_Camera.look(event.motion.xrel, event.motion.yrel);
            }
            break;
        }
    }
}

void Game::Update()
{
    m_Camera.move(Application::Get().m_LastFrameTime);
    auto time =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();
    // double t = fmod(time, 1000000.0) / 100000.0 * 2 * M_PI;
    double t = 1;
    m_LightPos = glm::vec3(sin(t) * 70.0, 80.0, cos(t) * 70.0);
}

void Game::Draw()
{
    GLCall(glEnable(GL_DEPTH_TEST));

    m_FrameBuffer.BindBuffers();

    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    m_TextureArray.Bind();

    m_World.BindVA();

    std::shared_ptr<Shader> CubeShader = m_ShaderLibrary.Get("CubeShader");
    CubeShader->Bind();
    CubeShader->SetUniformMat4fv("model", glm::mat4(1.0f));
    CubeShader->SetUniformMat4fv("view", m_Camera.getView());
    CubeShader->SetUniformMat4fv("MVP", m_Camera.getMVP());
    CubeShader->SetUniform1i("textureSlot", 1);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, m_World.numVertices()));

    m_ShadowBuffer.Bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    std::shared_ptr<Shader> DepthShader = m_ShaderLibrary.Get("DepthShader");
    DepthShader->Bind();
    glm::mat4 lightProjection = glm::perspective(
        glm::radians(90.0f),
        float(Application::Get().m_Width) / Application::Get().m_Height, 0.1f,
        1000.0f);
    glm::mat4 lightView =
        glm::lookAt(m_LightPos, glm::vec3(0, 65, 10), glm::vec3(0.0, 1.0, 0.0));
    DepthShader->SetUniformMat4fv(
        "LightSpaceMatrix", lightProjection * lightView);
    GLCall(glDrawArrays(GL_TRIANGLES, 0, m_World.numVertices()));

    GLCall(glDisable(GL_DEPTH_TEST)); // start deferred renderer

    m_ScreenQuad.Bind();

    m_FrameBuffer.BindTexture(GL_TEXTURE0, GL_COLOR_ATTACHMENT0);
    m_FrameBuffer.BindTexture(GL_TEXTURE1, GL_COLOR_ATTACHMENT1);
    m_FrameBuffer.BindTexture(GL_TEXTURE2, GL_COLOR_ATTACHMENT2);
    m_FrameBuffer.BindTexture(GL_TEXTURE3, GL_DEPTH_ATTACHMENT);
    m_ShadowBuffer.BindTexture(GL_TEXTURE5, GL_DEPTH_ATTACHMENT);

    // GLCall(glClearColor(0.0f, 0.0f, 0.4f, 0.0f));
    // GLCall(glClear(GL_COLOR_BUFFER_BIT));

    if (m_Occlusion)
    {
        glActiveTexture(GL_TEXTURE4);
        GLCall(glBindTexture(GL_TEXTURE_2D, texNoise));

        std::shared_ptr<Shader> SSAOShader = m_ShaderLibrary.Get("SSAOShader");
        SSAOShader->Bind();
        SSAOShader->SetUniform1i("gNormal", 1);
        SSAOShader->SetUniform1i("gPosition", 2);
        SSAOShader->SetUniform1i("texNoise", 5);
        SSAOShader->SetUniformMat4fv(
            "Projection",
            glm::perspective(
                glm::radians(90.0f),
                (float)m_Instance->m_Width / (float)m_Instance->m_Height, 0.1f,
                100.0f));
        for (int i = 0; i < 32; i++)
        {
            SSAOShader->SetUniform3f(
                "samples[" + std::to_string(i) + "]", samples[i]);
        }

        m_SSAOFrameBuffer.BindBuffers();
        GLCall(glDrawElements(
            GL_TRIANGLES, m_ScreenQuad.IndexCount(), GL_UNSIGNED_INT, nullptr));
        m_SSAOFrameBuffer.BindTexture(GL_TEXTURE4, GL_COLOR_ATTACHMENT0);
        m_SSAOFrameBuffer.Unbind();

        std::shared_ptr<Shader> SSAOBlurShader =
            m_ShaderLibrary.Get("SSAOBlurShader");
        SSAOBlurShader->Bind();
        SSAOBlurShader->SetUniform1i("ssaoTexture", 4);

        m_SSAOBlurFrameBuffer.BindBuffers();
        GLCall(glDrawElements(
            GL_TRIANGLES, m_ScreenQuad.IndexCount(), GL_UNSIGNED_INT, nullptr));
        m_SSAOBlurFrameBuffer.BindTexture(GL_TEXTURE4, GL_COLOR_ATTACHMENT0);
    }
    m_SSAOBlurFrameBuffer.Unbind();

    std::shared_ptr<Shader> ScreenShader = m_ShaderLibrary.Get("ScreenShader");
    ScreenShader->Bind();
    ScreenShader->SetUniform1i("gAlbedo", 0);
    ScreenShader->SetUniform1i("gNormal", 1);
    ScreenShader->SetUniform1i("gPosition", 2);
    ScreenShader->SetUniform1i("gDepth", 3);
    ScreenShader->SetUniform1i("ssaoTexture", 4);
    ScreenShader->SetUniform1i("shadowTexture", 5);
    glm::vec3 lightPosView =
        glm::vec3(m_Camera.getView() * glm::vec4(m_LightPos, 1.0));

    ScreenShader->SetUniformMat4fv(
        "inverseMVP", glm::inverse(m_Camera.getMVP()));
    ScreenShader->SetUniformMat4fv("lightMVP", lightProjection * lightView);
    ScreenShader->SetUniform3f("lightPos", lightPosView);
    ScreenShader->SetUniform1i("occlusion", m_Occlusion);

    // std::shared_ptr<Shader> BasicScreenShader =
    // m_ShaderLibrary.Get("BasicScreenShader"); BasicScreenShader->Bind();
    // BasicScreenShader->SetUniform1i("buffer", 3);

    GLCall(glDrawElements(
        GL_TRIANGLES, m_ScreenQuad.IndexCount(), GL_UNSIGNED_INT, nullptr));

    if (debug_fps)
    {
        using namespace std::chrono_literals;
        if (std::chrono::high_resolution_clock::now() > p1 + 125ms)
        {
            p1 = std::chrono::high_resolution_clock::now();
            auto duration = p1 - p2;
            m_FpsCounter.SetString(std::format(
                "{:.0f}fps\nlast frame time: {:.2f}ms",
                1.0 / std::chrono::duration_cast<std::chrono::duration<double>>(
                          duration)
                          .count(),
                std::chrono::duration_cast<
                    std::chrono::duration<double, std::milli>>(duration)
                    .count()));
        }
        m_FpsCounter.RenderText();
    }

    p2 = std::chrono::high_resolution_clock::now();
    SDL_GL_SwapWindow(Application::Get().m_Window);
}
