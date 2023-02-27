#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
    : m_Width(1920), m_Height(1080)
{
    s_Instance = this;
    // needs to be simplified

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_Window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
    m_Context = SDL_GL_CreateContext(m_Window);

    std::cout << SDL_GL_SetSwapInterval(0); // vsync

    SDL_Surface* icon;
    SDL_RWops* rwop;
    rwop = SDL_RWFromFile("res/marcotriangle.png", "rb");
    icon = IMG_LoadPNG_RW(rwop);
    SDL_SetWindowIcon(m_Window, icon);
    SDL_FreeSurface(icon);

    glewInit();
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
        std::chrono::duration<double> duration(p1-p2);
        m_LastFrameTime = duration.count()/100000000; // convert to S
        p2 = p1;
    }
}