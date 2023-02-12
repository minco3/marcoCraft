#include "Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
    : m_Width(1920), m_Height(1080)
{
    s_Instance = this;
    // needs to be simplified
    bool mouseVisible = true, lastMouseVisible = true;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetSwapInterval(1); // vsync

    glewInit();
}

Application::~Application()
{
    SDL_GL_DeleteContext(m_Context);
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void Application::RunImpl()
{
    SDL_Window* m_Window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
    SDL_GLContext m_Context = SDL_GL_CreateContext(m_Window);

    SDL_Surface* icon;
    SDL_RWops*  rwop;
    rwop = SDL_RWFromFile("res/marcotriangle.png", "rb");
    icon = IMG_LoadPNG_RW(rwop);
    SDL_SetWindowIcon(m_Window, icon);
    SDL_FreeSurface(icon);

    Game game;

    while (game.isRunning())
    {
        game.Event();
        game.Update();
        game.Render();
    }
}