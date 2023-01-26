#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_stdinc.h>

class Camera {
    public:
        Camera();
        
        void move(float deltaTime);
        void look(Sint32 xrel, Sint32 yrel);
        glm::mat4 getView();

        bool forward = false;
        bool back = false;
        bool left = false;
        bool right = false;
        bool up = false;
        bool down = false;
        bool sprint = false;
        double sensitivity = 500/2.5;
    
    private:
        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
};