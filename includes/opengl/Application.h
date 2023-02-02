#pragma once

class Application
{
public:
    static Application& Get() { return *s_Instance; }

private:
    static Application* s_Instance;
};