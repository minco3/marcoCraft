#shader vertex
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 coords;

out vec3 textureCoords;
out vec3 fragmentColor;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
    textureCoords = coords;
    fragmentColor = color;
}

#shader fragment
#version 330 core

in vec3 textureCoords;
in vec3 fragmentColor;
out vec4 color;

uniform sampler2DArray textureSlot;

void main()
{
    color = texture(textureSlot, textureCoords);
}