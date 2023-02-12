#include "Application.h"


Application::Application()
{
    SimplexNoise worldgen;

    const int width = 1920, height = 1080;

    bool mouseVisible = true, lastMouseVisible = true;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("Marcocraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    SDL_GL_SetSwapInterval(1); // vsync

    SDL_Surface* icon;
    SDL_RWops*  rwop;
    rwop = SDL_RWFromFile("res/marcotriangle.png", "rb");
    icon = IMG_LoadPNG_RW(rwop);
    SDL_SetWindowIcon(window, icon);


    glewInit();
}

void Application::Run()
{

}