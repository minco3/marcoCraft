#include "Camera.h"

#include <iostream>

Camera::Camera() 
    : m_cameraFront(1.0f, 0.0f, 0.0f), m_cameraPos(0.0f, 0.5f, 0.0f), m_cameraUp(0.0f, 1.0f, 0.0f) {}

void Camera::move(float deltaTime)
{
    // std::cout << deltaTime << "\n";
    glm::vec3 cameraVelocity(0.0f, 0.0f, 0.0f);
    if (back) cameraVelocity -= m_cameraFront;
    if (forward) cameraVelocity += m_cameraFront;
    if (left) cameraVelocity +=  glm::vec3(m_cameraFront.z, 0, -m_cameraFront.x);
    if (right) cameraVelocity += glm::vec3(-m_cameraFront.z, 0, m_cameraFront.x); 
    if (up) cameraVelocity.y += 1.0f; 
    if (down) cameraVelocity.y += -1.0f;

    cameraVelocity *=  glm::vec3(2.0f, 2.0f, 2.0f);

    if (sprint) cameraVelocity *= glm::vec3(4.0f, 4.0f, 4.0f);

    m_cameraPos += cameraVelocity*glm::vec3(deltaTime/1000,deltaTime/1000,deltaTime/1000);
}

void Camera::look(Sint32 xrel, Sint32 yrel)
{
    m_cameraFront = glm::normalize(
        glm::vec3(m_cameraFront.x*cos(xrel/sensitivity)-m_cameraFront.z*sin(xrel/sensitivity),
                  m_cameraFront.y-yrel/sensitivity,
                  m_cameraFront.x*sin(xrel/sensitivity)+m_cameraFront.z*cos(xrel/sensitivity))
    );
}

void Camera::setPosition(glm::vec3 position)
{
    m_cameraPos = position + glm::vec3(0,0.5,0);
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
    return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
}

glm::mat4 Camera::getMVP()
{
    glm::mat4 View = getView();
    glm::mat4 Model = glm::mat4(1.0f);
    return m_Projection * View * Model;
}