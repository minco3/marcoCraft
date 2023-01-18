#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include "includes/utils/a.h"

bool initGL();

int main(int argc, char** argv) {

    const int width = 1000, height = 1000;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, float(width)/float(height), 0.1, 100.0);

    SDL_GL_SetSwapInterval(1);

    glClearColor(0, 0, 0, 1);

    glClear(GL_COLOR_BUFFER_BIT);
    
    glPushMatrix();

    glTranslated(0, 0, -2);
    glRotatef(90, 0, 0, -1);

    glBegin(GL_POLYGON);
        glColor3f(0,0,1); glVertex3f(0.75,0.9, -1);
        glColor3f(0,0,1); glVertex3f(0,0.75, 0);
        glColor3f(0,1,0); glVertex3f(0.6,-0.75, 0);
        glColor3f(1,0,0); glVertex3f(-0.6,-0.75, 0);
    glEnd();

    glPopMatrix();

    glRectf (-0.75, -0.75, 0.75, 0.75);

    SDL_GL_SwapWindow(window);




    // SDL_Vertex vertecies[3] = {{{0,0}, {0,255,0,255}}, {{0,100}, {255,0,0,255}}, {{100,100}, {0,0,255,255}}}; 
    // SDL_RenderGeometry(renderer, nullptr, vertecies, 3, nullptr, 0);

    SDL_Event event;
    bool running = true;
    
    while (running) {
        //process all events
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_KEYDOWN:
                std::cout << (char)event.key.keysym.sym << '\n';
                if (event.key.keysym.sym == 'x') running = false;
            }
        }

        //render
        Uint64 lastTick = SDL_GetTicks64();
        // SDL_RenderPresent(renderer);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool initGL() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return true;
}