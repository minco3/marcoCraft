#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL_stdinc.h>

class Camera {
    public:
        Camera();
        
        void move(float deltaTime);
        void look(Sint32 xrel, Sint32 yrel);
        
        void SetScreenSize(float width, float height);
        void UpdateProjection();
        
        glm::mat4 getView();
        glm::mat4 getMVP();

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
        glm::mat4 m_Projection;
        float m_ScreenWidth, m_ScreenHeight;
};