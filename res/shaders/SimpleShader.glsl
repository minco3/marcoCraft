#shader vertex
#version 330 core

out vec3 fragmentColor;

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    fragmentColor = vertexColor;
}

#shader fragment
#version 330 core
in vec3 fragmentColor;
out vec3 color;
void main()
{
    color = fragmentColor;
}