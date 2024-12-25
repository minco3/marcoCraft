#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
    : m_Width(1280), m_Height(720)
{
    s_Instance = this;
    // needs to be simplified

    SDL_Init(SDL_INIT_VIDEO);

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    m_Window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_Context = SDL_GL_CreateContext(m_Window);
    SDL_GL_MakeCurrent(m_Window, m_Context);

    SDL_GL_SetSwapInterval(0); // vsync

    SDL_Surface* icon;
    SDL_RWops* rwop;
    rwop = SDL_RWFromFile("res/marcotriangle.png", "rb");
    icon = IMG_LoadPNG_RW(rwop);
    SDL_SetWindowIcon(m_Window, icon);
    SDL_FreeSurface(icon);

    glewExperimental = GL_TRUE;
    auto err = glewInit();

    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "OpenGL version: " << version << std::endl;
}

Application::~Application()
{
    SDL_GL_DeleteContext(m_Context);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void Application::Run()
{
    Game game;

    while (game.isRunning())
    {
        std::chrono::high_resolution_clock::time_point p1, p2;
        p1 = std::chrono::high_resolution_clock::now();
        game.Event();
        game.Update();
        game.Draw();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(p1-p2);
        m_LastFrameTime = duration.count()/static_cast<float>(std::chrono::high_resolution_clock::period().den); // convert to S
        p2 = p1;
    }
}