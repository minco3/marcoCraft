#include "camera.h"

Camera::Camera() :
    cameraFront(0.0f, 0.0f, -1.0f),
    cameraPos(0.0f, 0.0f, 3.0f),
    cameraUp(0.0f, 1.0f, 0.0f) {}

void Camera::move(float deltaTime) {
    glm::vec3 cameraVelocity(0.0f, 0.0f, 0.0f);
    if (back) cameraVelocity -= cameraFront;
    if (forward) cameraVelocity += cameraFront;
    if (left) cameraVelocity +=  glm::vec3(cameraFront.z, 0, -cameraFront.x);
    if (right) cameraVelocity += glm::vec3(-cameraFront.z, 0, cameraFront.x); 
    if (up) cameraVelocity.y += 1.0f; 
    if (down) cameraVelocity.y += -1.0f;

    

    cameraPos += cameraVelocity*glm::vec3(deltaTime,deltaTime,deltaTime);
}

void Camera::look(Sint32 xrel, Sint32 yrel) {
    cameraFront = glm::normalize(
        glm::vec3(cameraFront.x*cos(xrel/(float)200)-cameraFront.z*sin(xrel/(float)200),
                  cameraFront.y-yrel/(float)200,
                  cameraFront.x*sin(xrel/(float)200)+cameraFront.z*cos(xrel/(float)200))
    );
}

glm::mat4 Camera::getView() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}