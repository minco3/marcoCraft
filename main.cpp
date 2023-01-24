#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>

#include "includes/utils/a.h"

int main(int argc, char** argv) {

    const int width = 1000, height = 1000;
    
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewInit();
    GLuint VertexArrayID;

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f,  1.0f, 0.0f,
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

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
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0,
            (void*)0
        );
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window);
        glDisableVertexAttribArray(0);
    }


    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}