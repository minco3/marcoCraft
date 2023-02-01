#include "camera.h"

Camera::Camera() 
    : cameraFront(0.0f, 0.0f, -1.0f), cameraPos(0.0f, 0.0f, 3.0f), cameraUp(0.0f, 1.0f, 0.0f) {}

void Camera::move(float deltaTime)
{
    glm::vec3 cameraVelocity(0.0f, 0.0f, 0.0f);
    if (back) cameraVelocity -= cameraFront;
    if (forward) cameraVelocity += cameraFront;
    if (left) cameraVelocity +=  glm::vec3(cameraFront.z, 0, -cameraFront.x);
    if (right) cameraVelocity += glm::vec3(-cameraFront.z, 0, cameraFront.x); 
    if (up) cameraVelocity.y += 1.0f; 
    if (down) cameraVelocity.y += -1.0f;

    if (sprint) cameraVelocity *= glm::vec3(2.0f, 2.0f, 2.0f);

    cameraPos += cameraVelocity*glm::vec3(deltaTime/1000,deltaTime/1000,deltaTime/1000);
}

void Camera::look(Sint32 xrel, Sint32 yrel)
{
    cameraFront = glm::normalize(
        glm::vec3(cameraFront.x*cos(xrel/sensitivity)-cameraFront.z*sin(xrel/sensitivity),
                  cameraFront.y-yrel/sensitivity,
                  cameraFront.x*sin(xrel/sensitivity)+cameraFront.z*cos(xrel/sensitivity))
    );
}

void Camera::SetScreenSize(float width, float height)
{
    m_ScreenWidth = width;
    m_ScreenHeight = height;
    UpdateProjection();
}

void Camera::UpdateProjection()
{
    m_Projection = glm::perspective(glm::radians(45.0f), (float)m_ScreenWidth / (float)m_ScreenHeight, 0.1f, 100.0f);
}

glm::mat4 Camera::getView()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

glm::mat4 Camera::getMVP()
{
    glm::mat4 View = getView();
    glm::mat4 Model = glm::mat4(1.0f);
    return m_Projection * View * Model;
}