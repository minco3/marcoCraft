#pragma once

class Application;
class Camera;
class TextureArray;
class VertexArray;
class VertexBuffer;


class Game {
public:

Game();

void Event();
void Update();
void Render();

bool isRunning() {return running;}

private:
    bool running;
    Application& m_Instance;
    Camera m_Camera;


    bool mouseVisible;
    bool fullscreen;
    bool debug_fps;
};